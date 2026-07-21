# `mini_serv` — Pseudocódigo y chuleta

---

## PARTE 1 — Lo del `2130706433`

**No hace falta memorizarlo.** Es `127.0.0.1` visto como un entero de 32 bits:

```
127  .  0  .  0  .  1
0x7F . 0x00 . 0x00 . 0x01     →    0x7F000001    →    2130706433
```

Verificado con `gcc -Wall -Wextra -Werror`, las cuatro producen **exactamente los mismos
bytes** (`0x0100007F` en memoria little-endian):

```c
servaddr.sin_addr.s_addr = htonl(2130706433);        // el que memoriza todo el mundo
servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);   // ★ EL MEJOR
servaddr.sin_addr.s_addr = htonl(0x7F000001);        // fácil de derivar mentalmente
servaddr.sin_addr.s_addr = htonl((127 << 24) | 1);   // si te bloqueas con el hex
```

### Usa `htonl(INADDR_LOOPBACK)`

`INADDR_LOOPBACK` ya está definido en `<netinet/in.h>` como `0x7f000001`. Ventajas:

- **No memorizas nada**, se lee solo.
- **No viola la regla de "no `#define`"**: la regla es que *tú* no escribas `#define` en tu
  archivo. Usar macros de las cabeceras del sistema es obligatorio de todas formas —
  `AF_INET`, `SOCK_STREAM`, `FD_SET`, `FD_ZERO` también son macros y nadie te dice nada.
- Es lo que sale en `man 7 ip`, así que es defendible ante cualquier evaluador.

### ⚠️ Lo que NO puedes hacer

```c
servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");   // ✗ inet_addr NO está permitida
servaddr.sin_addr.s_addr = 0x0100007F;               // ✗ funciona pero SOLO en little-endian
```

El segundo es el valor ya en orden de red, así que "funciona" en tu portátil y en el del
examen (x86), pero es *undefined* conceptualmente y no sabrías explicarlo. No lo hagas.

### ¿Y `htons` / `htonl`? No están en la lista de permitidas

Correcto, no aparecen. En la práctica **se aceptan siempre** (son macros/inline de la libc,
igual que `FD_SET`). Todo el mundo aprueba con ellas. Si te entra el pánico, `htons(port)`
para un puerto se puede hacer a mano:

```c
int p = atoi(argv[1]);
servaddr.sin_port = ((p & 0xFF) << 8) | ((p >> 8) & 0xFF);   // swap de 2 bytes
```

Pero de verdad: **usa `htons`/`htonl`**. No te compliques.

---

## PARTE 2 — Pseudocódigo completo

### Estado global

```
count        = 0          // siguiente ID libre. SOLO SUBE, nunca se reutiliza
max_fd       = 0          // fd más alto vivo
sockfd       = -1         // socket de escucha

ids[fd]      : int        // id lógico del cliente que ocupa ese fd
msgs[fd]     : char*      // BUFFER DE ENTRADA: lo recibido sin terminar en \n
to_send[fd]  : char*      // BUFFER DE SALIDA: lo pendiente de mandarle
to_send_len  : int        // bytes útiles en to_send[fd]
to_send_off  : int        // cuántos de esos ya salieron (para send parciales)
to_send_cap  : int        // memoria reservada (crece x2)

all_fds      : fd_set     // LA VERDAD: todos los fds vivos
read_fds, write_fds       // copias de trabajo; select las DESTRUYE cada vuelta
```

### `main`

```
SI numero_de_argumentos != 2:
    escribir en stderr "Wrong number of arguments\n"    (26 bytes)
    salir(1)

FD_ZERO(all_fds)
sockfd = socket(AF_INET, SOCK_STREAM, 0)
SI sockfd < 0: fatal_error()
FD_SET(sockfd, all_fds)
max_fd = sockfd

poner a cero la struct servaddr        (bzero)
servaddr.familia   = AF_INET
servaddr.direccion = htonl(INADDR_LOOPBACK)
servaddr.puerto    = htons(atoi(argv[1]))

SI bind(...)   falla: fatal_error()
SI listen(sockfd, 128) falla: fatal_error()

BUCLE INFINITO:

    ── (A) preparar los sets ────────────────────────────────
    read_fds = all_fds                    // interesa leer de TODOS
    FD_ZERO(write_fds)
    PARA cada fd de 0 a max_fd:
        SI to_send[fd] != NULL:           // ← SOLO los que tienen algo pendiente
            FD_SET(fd, write_fds)         //   (si no: busy-loop al 100% CPU)

    ── (B) dormir hasta que pase algo ───────────────────────
    SI select(max_fd + 1, read_fds, write_fds, NULL, NULL) < 0:
        fatal_error()

    ── (C) atender lecturas ─────────────────────────────────
    PARA cada fd de 0 a max_fd:
        SI fd no está en read_fds: continuar

        SI fd == sockfd:                          // conexión entrante
            nuevo = accept(sockfd, NULL, NULL)
            SI nuevo >= 0:
                register_client(nuevo)
            ROMPER bucle          // all_fds y max_fd cambiaron → rehacer select

        SI NO:                                    // datos de un cliente
            n = recv(fd, buf_read, 1000, 0)
            SI n <= 0:                            // se desconectó
                remove_client(fd)
                ROMPER bucle                      // ídem
            buf_read[n] = '\0'
            msgs[fd] = str_join(msgs[fd], buf_read)
            SI msgs[fd] == NULL: fatal_error()
            send_msg(fd)

    ── (D) vaciar colas de salida ───────────────────────────
    PARA cada fd de 3 a max_fd:
        SI fd != sockfd: flush_client(fd)
```

> **Por qué los `ROMPER`**: tras un `accept` o un `close`, `all_fds`/`max_fd` han cambiado y
> `read_fds` ya está obsoleto. Se sale y se rehace `select`. Como `select` es
> *level-triggered*, lo que quedara pendiente se vuelve a reportar en la siguiente vuelta.
> **No se pierde nada.**

### Las funciones auxiliares

```
fatal_error():
    escribir en stderr "Fatal error\n"    (12 bytes)
    salir(1)


register_client(fd):
    SI fd > max_fd: max_fd = fd
    ids[fd] = count            // asignar ID...
    count = count + 1          // ...y avanzar el contador
    msgs[fd] = NULL
    to_send[fd] = NULL;  to_send_len = to_send_off = to_send_cap = 0
    FD_SET(fd, all_fds)
    formatear "server: client %d just arrived\n" con ids[fd]
    notify_other(fd, ese_texto)

    // el reset de to_send_* es OBLIGATORIO: este fd pudo ser de un cliente anterior


remove_client(fd):
    formatear "server: client %d just left\n" con ids[fd]
    notify_other(fd, ese_texto)         // avisar ANTES de cerrar
    liberar msgs[fd]      ;  msgs[fd] = NULL
    liberar to_send[fd]   ;  to_send[fd] = NULL
    to_send_len = to_send_off = to_send_cap = 0
    FD_CLR(fd, all_fds)
    close(fd)


notify_other(autor, texto):
    PARA cada fd de 0 a max_fd:
        SI fd está en all_fds  Y  fd != autor  Y  fd != sockfd:
            queue_msg(fd, texto)        // encolar, NO enviar

    // ojo: recorre all_fds (los VIVOS), no write_fds (los escribibles AHORA).
    // Ésa es la diferencia entre perder mensajes y no perderlos.


send_msg(fd):                           // drenar TODAS las líneas completas
    ret = extract_message(&msgs[fd], &linea)
    MIENTRAS ret > 0:
        formatear "client %d: " con ids[fd]
        notify_other(fd, ese_prefijo)   // 1) el prefijo
        notify_other(fd, linea)         // 2) la línea (ya trae su \n)
        liberar linea
        ret = extract_message(&msgs[fd], &linea)
    SI ret < 0: fatal_error()           // el calloc de dentro falló

    // MIENTRAS, no SI: un solo recv puede traer 5 líneas.
    // Prefijo DENTRO del bucle: una vez POR LÍNEA, no por recv.


queue_msg(fd, texto):                   // añadir a la cola de salida
    addlen  = longitud(texto)
    needed  = to_send_len[fd] + addlen + 1        // +1 por el '\0'
    SI needed > to_send_cap[fd]:
        newcap = to_send_cap[fd] * 2
        SI newcap < needed: newcap = needed       // cubre el caso cap == 0
        nuevo = realloc(to_send[fd], newcap)
        SI nuevo == NULL: fatal_error()
        to_send[fd]     = nuevo
        to_send_cap[fd] = newcap
    copiar texto al final de to_send[fd]  (en la posición to_send_len[fd])
    to_send_len[fd] += addlen

    // duplicar la capacidad en vez de ajustar exacto:
    // realloc solo mueve memoria O(log n) veces → append O(len) amortizado
    // en vez de O(backlog) por llamada.


flush_client(fd):                       // enviar lo que el socket acepte AHORA
    SI to_send[fd] == NULL  O  fd no está en write_fds: volver
    restantes = to_send_len[fd] - to_send_off[fd]
    enviados  = send(fd, to_send[fd] + to_send_off[fd], restantes, 0)
    SI enviados <= 0: volver            // NO desconectar: puede ser un cliente lazy
    to_send_off[fd] += enviados
    SI to_send_off[fd] == to_send_len[fd]:      // cola vacía → liberar
        liberar to_send[fd] ; to_send[fd] = NULL
        to_send_len = to_send_off = to_send_cap = 0

    // to_send_off evita tener que mover los bytes restantes al principio
    // con un memmove O(n) en cada send parcial.
```

---

## PARTE 3 — El flujo de un mensaje, paso a paso

Cliente 1 escribe `hola\nadios\n`, hay conectados el 0 y el 2:

```
1. select dice: fd del cliente 1 es legible
2. recv → buf_read = "hola\nadios\n"
3. str_join → msgs[fd1] = "hola\nadios\n"      (se acumula por si venía partido)
4. send_msg:
   4a. extract_message → linea="hola\n"   msgs[fd1]="adios\n"
       notify_other("client 1: ")  → queue_msg a fd0 y fd2
       notify_other("hola\n")      → queue_msg a fd0 y fd2
   4b. extract_message → linea="adios\n" msgs[fd1]=""
       notify_other("client 1: ")  → queue_msg a fd0 y fd2
       notify_other("adios\n")     → queue_msg a fd0 y fd2
   4c. extract_message → 0, no hay más \n → salir del while
5. to_send[fd0] = to_send[fd2] = "client 1: hola\nclient 1: adios\n"
6. flush_client(fd0), flush_client(fd2) → send al que esté escribible
7. Si uno solo acepta 10 bytes → to_send_off=10, el resto se manda la vuelta siguiente
```

Si el cliente hubiera escrito `hol` y luego `a\n`:
paso 3 deja `msgs[fd1]="hol"`, `extract_message` devuelve 0, **no se envía nada**.
Al siguiente `recv`, `msgs[fd1]="hola\n"` y ya sale. Correcto según el enunciado.

---

## PARTE 4 — Chuleta de un vistazo

| Cosa | Valor |
|---|---|
| `"Wrong number of arguments\n"` | **26** bytes |
| `"Fatal error\n"` | **12** bytes |
| `127.0.0.1` | `htonl(INADDR_LOOPBACK)` — sin memorizar nada |
| Primer arg de `select` | `max_fd + 1` (número de fds, no índice) |
| `recv` | `recv(fd, buf, 1000, 0)` con `buf[1001]` (hueco para `\0`) |
| Backlog de `listen` | 128 (cualquiera vale) |

**Orden de escritura en el examen** (compila en cada paso):

1. Copiar `extract_message` y `str_join` del `main.c` que te dan → 5 min
2. `main`: args, socket, bind, listen, `select` loop, `accept`, `recv` → 15 min
3. `ids` + `msgs` + `notify_other` con `send` directo → ya pasas casi todo → 10 min
4. Si sobra: `to_send` + `flush_client` para los lazy → 15 min
5. `write_fds` selectivo + checklist de tests → 5 min

**Fallos que más caen:**

- Olvidar `exit(1)` tras "Wrong number of arguments"
- Mandarle el mensaje también al autor
- Poner el prefijo una vez por `recv` en lugar de una por línea
- Usar `fd` como ID en vez de un contador global (los IDs **no** se reutilizan)
- No reasignar `read_fds = all_fds` dentro del `while` (`select` destruye los sets)
- `select(max_fd, ...)` en vez de `max_fd + 1`
- Dejarte un `printf` de debug → función prohibida → 0
