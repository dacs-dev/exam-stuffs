# Exam Rank 06 — `mini_serv` — Guía de estudio rápida

> Objetivo: entender qué hace cada línea, saber reescribirlo de cero en ~25 min,
> y conocer los fallos que el corrector puede pillar.

---

## 0. El enunciado en 10 líneas

Servidor de chat TCP en `127.0.0.1`, puerto = `argv[1]`.

| Situación | Qué hacer |
|---|---|
| `argc != 2` | `write(2, "Wrong number of arguments\n", 26)` + `exit(1)` |
| Falla una syscall antes de aceptar | `write(2, "Fatal error\n", 12)` + `exit(1)` |
| Falla `malloc`/`calloc`/`realloc` | `Fatal error` + `exit(1)` |
| Cliente conecta | a **los demás**: `server: client %d just arrived\n` |
| Cliente manda línea | a **los demás**: `client %d: ` + la línea (¡prefijo por CADA línea!) |
| Cliente se va | a **los demás**: `server: client %d just left\n` |

Reglas duras:
- IDs incrementales: 0, 1, 2… (**nunca se reutilizan**, aunque el fd sí).
- Sin `#define`. Sin leaks de memoria ni de fds.
- Cliente "lazy" (que no lee) **NO se desconecta**.
- Sólo `select()` — nada de `poll`/`epoll`/threads.

Funciones permitidas: `write, close, select, socket, accept, listen, send, recv, bind,
strstr, malloc, realloc, free, calloc, bzero, atoi, sprintf, strlen, exit, strcpy,
strcat, memset`.

> ⚠️ `htons`/`htonl` **no están en la lista** pero se aceptan universalmente (son
> macros/inline de la libc). Si te da miedo, en x86 puedes hacer el byte-swap a mano,
> pero yo lo dejaría con `htons`.

---

## 1. Los archivos del repo

| Archivo | Qué es |
|---|---|
| `main.c` | El esqueleto que **te da el enunciado**. Usa `printf` (prohibida) y sólo hace 1 `accept`. Sirve para copiar `extract_message` y `str_join`, nada más. |
| `mini_serv2.c` | **Versión ingenua**: `send()` directo dentro del bucle. Corta y simple. Tiene bugs (§6). |
| `mini_serv.c` | **Versión buena**: cola de salida por cliente con `realloc` amortizado y offset para `send` parciales. Es la que hay que entender. |
| `a.out` | Binario compilado, ignóralo. |

---

## 2. Las dos funciones regaladas (memorízalas, se copian tal cual)

### `extract_message(char **buf, char **msg)`

Saca **la primera línea completa** (hasta el primer `\n` incluido) del buffer acumulado
de un cliente.

```c
int extract_message(char **buf, char **msg)
{
    char *newbuf;
    int   i;

    *msg = 0;
    if (*buf == 0)
        return (0);
    i = 0;
    while ((*buf)[i])
    {
        if ((*buf)[i] == '\n')
        {
            newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
            if (newbuf == 0)
                return (-1);            // ← malloc falló
            strcpy(newbuf, *buf + i + 1);  // el resto (después del \n)
            *msg = *buf;                   // el original pasa a ser el mensaje
            (*msg)[i + 1] = 0;             // lo cortamos justo tras el \n
            *buf = newbuf;                 // el buffer del cliente = el resto
            return (1);
        }
        i++;
    }
    return (0);   // no hay \n todavía → no hay línea completa
}
```

Devuelve:
- `1` → hay línea; `*msg` es tuyo (**tienes que hacer `free`**), `*buf` es el sobrante.
- `0` → aún no hay `\n` completo. No tocar nada.
- `-1` → `calloc` falló → `Fatal error`.

Truco mental: **rompe la cadena en dos**. La parte de delante (con su `\n`) se convierte
en `msg`; la parte de detrás se copia a un buffer nuevo que pasa a ser el `buf` del cliente.

### `str_join(char *buf, char *add)`

Concatena y **libera el viejo**. Es un `strdup(strcat(buf, add))` con `free(buf)` dentro.

```c
char *str_join(char *buf, char *add)
{
    char *newbuf;
    int   len;

    len = (buf == 0) ? 0 : strlen(buf);
    newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
    if (newbuf == 0)
        return (0);          // ← ojo: NO libera buf. Aquí SÍ hay un leak técnico,
    newbuf[0] = 0;           //   pero como llamas a Fatal error y sales, da igual.
    if (buf != 0)
        strcat(newbuf, buf);
    free(buf);
    strcat(newbuf, add);
    return (newbuf);
}
```

Uso: `msgs[fd] = str_join(msgs[fd], buf_read);` — el puntero viejo ya está liberado,
no lo vuelvas a usar.

---

## 3. `mini_serv.c` — estado global

```c
int   count = 0;          // siguiente ID a repartir (¡nunca decrece!)
int   max_fd = 0;         // fd más alto vivo → primer argumento de select()
int   sockfd = -1;        // socket de escucha
int   ids[65536];         // ids[fd] = id lógico del cliente en ese fd
char *msgs[65536];        // buffer de ENTRADA por fd (líneas incompletas)

char *to_send[65536];     // buffer de SALIDA por fd (lo pendiente de enviar)
int   to_send_len[65536]; // bytes válidos dentro de to_send[fd]
int   to_send_off[65536]; // cuántos ya se enviaron (para send parciales)
int   to_send_cap[65536]; // capacidad reservada (para el crecimiento x2)

fd_set read_fds, write_fds, all_fds;
char buf_read[1001], buf_write[42];
```

**Por qué arrays indexados por fd**: el kernel te da siempre el fd libre más bajo, así que
`ids[fd]`, `msgs[fd]`… es un "hashmap" O(1) gratis. No hace falta lista enlazada.

- `all_fds` = la verdad: todos los fds vivos (listening + clientes).
- `read_fds` / `write_fds` = copias que `select()` **destruye** en cada vuelta.
  Por eso se reasignan siempre antes de llamar: `read_fds = write_fds = all_fds;`.

---

## 4. Función por función

### `fatal_error()`
`write(2, "Fatal error\n", 12); exit(1);` — nada más. 12 es el `strlen`, cuéntalo bien.

### `create_socket()`
```c
max_fd = socket(AF_INET, SOCK_STREAM, 0);   // TCP/IPv4
if (max_fd < 0) fatal_error();
FD_SET(max_fd, &all_fds);
return max_fd;
```
Mete el socket de escucha en `all_fds` (así `select` avisa cuando hay conexión entrante)
e inicializa `max_fd`.

### `queue_msg(int fd, char *str)` — **el corazón de la versión buena**
Añade `str` a la cola de salida de `fd`. No envía nada.

```c
addlen = strlen(str);
needed = to_send_len[fd] + addlen + 1;     // +1 por el '\0'
if (needed > to_send_cap[fd])
{
    newcap = to_send_cap[fd] * 2;          // crecimiento geométrico
    if (newcap < needed) newcap = needed;  // cubre el caso cap == 0
    newbuf = realloc(to_send[fd], newcap);
    if (newbuf == 0) fatal_error();
    to_send[fd] = newbuf;
    to_send_cap[fd] = newcap;
}
strcpy(to_send[fd] + to_send_len[fd], str);
to_send_len[fd] += addlen;
```

**Por qué duplicar la capacidad**: si hicieras `realloc(len + addlen + 1)` exacto en cada
append, cada mensaje copiaría todo el backlog → O(n²) con clientes lentos. Duplicando,
`realloc` sólo mueve memoria O(log n) veces y cada append es O(len(str)) amortizado.

### `flush_client(int fd)` — envía lo que quepa ahora mismo
```c
if (to_send[fd] == 0 || !FD_ISSET(fd, &write_fds)) return;
remaining = to_send_len[fd] - to_send_off[fd];
sent = send(fd, to_send[fd] + to_send_off[fd], remaining, 0);
if (sent <= 0) return;                 // no escribible / error → reintentar luego
to_send_off[fd] += sent;
if (to_send_off[fd] == to_send_len[fd])   // vaciado del todo → liberar
{
    free(to_send[fd]); to_send[fd] = 0;
    to_send_len[fd] = to_send_off[fd] = to_send_cap[fd] = 0;
}
```

**Por qué `to_send_off`**: `send()` puede escribir sólo parte. En vez de mover los bytes
restantes al principio con un `memmove` (O(n) cada vez), guardas cuánto llevas enviado.
Esto es lo que resuelve el requisito de "cliente lazy": si no lee, `send` devuelve poco o
`-1`, tú no lo desconectas, sólo lo dejas en la cola.

### `notify_other(int author, char *str)`
```c
for (int fd = 0; fd <= max_fd; fd++)
    if (FD_ISSET(fd, &all_fds) && fd != author && fd != sockfd)
        queue_msg(fd, str);
```
Encola en **todos menos el autor y el socket de escucha**. Nota que itera sobre `all_fds`
(los vivos), no sobre `write_fds` — esa es la diferencia clave con `mini_serv2.c`.

### `register_client(int fd)`
```c
if (fd > max_fd) max_fd = fd;
ids[fd] = count++;               // ← el ID se asigna aquí y nunca se reutiliza
msgs[fd] = NULL;
to_send[fd] = NULL; to_send_len[fd] = to_send_off[fd] = to_send_cap[fd] = 0;
FD_SET(fd, &all_fds);
sprintf(buf_write, "server: client %d just arrived\n", ids[fd]);
notify_other(fd, buf_write);
```
El reset de todos los `to_send_*` es **obligatorio**: el fd pudo pertenecer a un cliente
anterior ya cerrado.

### `remove_client(int fd)`
```c
sprintf(buf_write, "server: client %d just left\n", ids[fd]);
notify_other(fd, buf_write);     // avisa a los demás (fd != author lo excluye a él)
free(msgs[fd]);                  // ← falta msgs[fd] = NULL (ver mejora 6)
free(to_send[fd]);
to_send[fd] = NULL; to_send_len[fd] = to_send_off[fd] = to_send_cap[fd] = 0;
FD_CLR(fd, &all_fds);
close(fd);
```

### `send_msg(int fd)` — drenar todas las líneas completas
```c
ret = extract_message(&(msgs[fd]), &msg);
while (ret > 0)
{
    sprintf(buf_write, "client %d: ", ids[fd]);
    notify_other(fd, buf_write);   // 1) el prefijo
    notify_other(fd, msg);         // 2) la línea (ya lleva su \n)
    free(msg);
    ret = extract_message(&(msgs[fd]), &msg);
}
if (ret < 0) fatal_error();
```
`while` porque un solo `recv` puede traer varias líneas. Cada una lleva **su propio
prefijo** → eso cumple el "prepend before each line".

### `main()`
```c
argc != 2 → "Wrong number of arguments\n" (26 bytes) + exit(1)
FD_ZERO(&all_fds); sockfd = create_socket();
servaddr.sin_family = AF_INET;
servaddr.sin_addr.s_addr = htonl(2130706433);   // 2130706433 == 0x7F000001 == 127.0.0.1
servaddr.sin_port = htons(atoi(argv[1]));
bind(...) != 0 → fatal_error();
listen(sockfd, 128) != 0 → fatal_error();

while (1) {
    read_fds = write_fds = all_fds;
    select(max_fd + 1, &read_fds, &write_fds, NULL, NULL) < 0 → fatal_error();

    for (fd = 0..max_fd) {
        if (!FD_ISSET(fd, &read_fds)) continue;
        if (fd == sockfd) { accept → register_client → break; }
        else {
            n = recv(fd, buf_read, 1000, 0);
            if (n <= 0) { remove_client(fd); break; }
            buf_read[n] = '\0';
            msgs[fd] = str_join(msgs[fd], buf_read);   // (con check de NULL)
            send_msg(fd);
        }
    }
    for (fd = 0..max_fd) if (fd != sockfd) flush_client(fd);
}
```

**Los `break`**: tras `accept` o tras cerrar un cliente, `all_fds`/`max_fd` han cambiado y
`read_fds` ya no es de fiar → se sale del bucle y se rehace `select`. `select` es
*level-triggered*, así que lo que quedaba pendiente se vuelve a reportar en la siguiente
vuelta. No se pierde nada.

**Por qué `max_fd + 1`**: `select` recibe el número de fds a mirar, no el índice máximo.

**Por qué `recv(..., 1000, ...)` con `buf_read[1001]`**: hueco para el `'\0'` de `buf_read[n]`.

---

## 5. Diagrama del flujo de datos

```
 red ──recv──> buf_read ──str_join──> msgs[fd]  (entrada, líneas parciales)
                                          │
                                    extract_message  (saca 1 línea completa)
                                          │
                                     "client N: " + línea
                                          │
                                     notify_other → queue_msg
                                          │
                              to_send[otro_fd]  (salida, con off/len/cap)
                                          │
                                    flush_client ──send──> red
```

Dos buffers por cliente: **uno de entrada** (`msgs`) porque TCP no respeta fronteras de
mensaje, y **uno de salida** (`to_send`) porque el cliente puede no leer.

---

## 6. Bugs y cosas a mejorar

### 🔴 BUG REAL 1 — `select` hace busy-loop (100% CPU)

`write_fds = all_fds` mete **todos** los clientes en el set de escritura. Un socket TCP
sin datos pendientes está *siempre* escribible → `select()` retorna inmediatamente en cada
vuelta, para siempre. Medido en este repo: **60% de CPU con el servidor en reposo.**

**Fix** (sustituye `read_fds = write_fds = all_fds;` en el `while`):

```c
read_fds = all_fds;
FD_ZERO(&write_fds);
for (int fd = 0; fd <= max_fd; fd++)
    if (to_send[fd] != 0)
        FD_SET(fd, &write_fds);
```

Con esto sólo pides "avísame cuando pueda escribir" para los que tienen algo que enviar.
CPU en reposo: 0%. Es la mejora nº1 y la que preguntaría un evaluador.

### 🔴 BUG REAL 2 — `mini_serv2.c` no sale con `argc != 2`

```c
if (ac != 2) { write(2, "Wrong number of arguments\n", 26); }   // ← falta exit(1)
...
servaddr.sin_port = htons(atoi(av[1]));                        // ← atoi(NULL) = segfault
```
Doble fallo: no cumple el enunciado (debe salir con status 1) **y** casca. Fijo en
`mini_serv.c`, pero si copias del 2 te lo llevas.

### 🔴 BUG REAL 3 — `mini_serv2.c` pierde mensajes

`notify_other` hace `send()` directo sólo `if (FD_ISSET(fd, &write_fds))`. Si un cliente
no está escribible en ese instante, **el mensaje se pierde silenciosamente**. Y aunque lo
esté, `send()` puede escribir parcialmente y el resto se descarta (no se comprueba el
retorno). Exactamente el escenario "lazy client" del enunciado. Ésta es la razón de ser de
toda la maquinaria `to_send` de `mini_serv.c`.

### 🟠 BUG REAL 4 — `mini_serv2.c`: bucle infinito si falla `calloc`

```c
while (extract_message(&(msgs[fd]), &msg))   // -1 también es "verdadero"
```
Si `extract_message` devuelve `-1`, la condición es cierta, el buffer no cambia, y vuelve a
devolver `-1` → cuelgue. `mini_serv.c` lo hace bien con `ret > 0` + `if (ret < 0) fatal_error()`.

### 🟡 Mejora 5 — `buf_write[42]` va justísimo

`"server: client 2147483647 just arrived\n"` = 39 chars + `\0` = 40. Cabe, pero por 2
bytes. Sube a `char buf_write[64];` y duermes tranquilo. (`sprintf` sin límite es una
bomba; aquí no explota, pero no la dejes armada.)

### 🟡 Mejora 6 — `msgs[fd]` queda colgando (dangling)

`remove_client` hace `free(msgs[fd])` pero no `msgs[fd] = NULL;`. Hoy es inocuo porque
`register_client` lo resetea al reutilizar el fd, pero es un doble-free esperando a que
alguien reordene el código. Añade la línea.

### 🟡 Mejora 7 — `accept()` machaca `servaddr`

```c
int client_fd = accept(sockfd, (struct sockaddr*)&servaddr, &addr_len);
```
Se reutiliza la struct del `bind`. Funciona (el bind ya se hizo) pero es sucio. Lo limpio
es `accept(sockfd, NULL, NULL)` — no necesitas la dirección del cliente para nada.

### 🟡 Mejora 8 — `FD_SETSIZE`

Los arrays son de 65536 pero `select` sólo maneja fds `< FD_SETSIZE` (1024 en Linux).
Pasar de ahí es **undefined behavior**. Para el examen da igual, pero si te preguntan
"¿por qué usarías `poll`?" → ésta es la respuesta.

### 🟢 Cosmético 9 — `flush_client` recorre fds 0,1,2

El bucle final va desde `fd = 0`, tocando stdin/stdout/stderr. Sale enseguida por
`to_send[fd] == 0`, pero podrías arrancar en `fd = 3`.

### 🟢 Cosmético 10 — leak en `str_join` si falla `malloc`

No libera `buf` antes de devolver `0`. Como el llamante hace `fatal_error()` y `exit(1)`,
el SO lo recupera. No lo toques: esa función es **código regalado**, cambiarla te expone a
introducir un bug real.

---

## 7. Checklist de compilación y test (hazlo antes de entregar)

```bash
gcc -Wall -Wextra -Werror mini_serv.c -o mini_serv    # debe compilar sin nada
./mini_serv                     # → "Wrong number of arguments", echo $? == 1
./mini_serv 8080 &

# terminal 2 y 3:
nc 127.0.0.1 8080
```

Comprueba a mano:
1. Cliente A conecta, cliente B conecta → A ve `server: client 1 just arrived`.
2. B escribe `hola` → A ve `client 1: hola`. **B NO se ve a sí mismo.**
3. `printf 'a\nb\nc\n' | nc 127.0.0.1 8080` → 3 líneas, cada una con su prefijo.
4. Escribe sin `\n` y espera → no se envía nada hasta el `\n`. Correcto.
5. Ctrl-C en B → A ve `server: client 1 just left`.
6. Reconecta B → recibe id **2**, no 1. (IDs no se reutilizan.)
7. `ps -o %cpu -C mini_serv` en reposo → debe ser ~0.0 (si no, tienes el bug 1).
8. `grep define mini_serv.c` → vacío.

---

## 8. Estrategia para el examen (2h)

**No escribas la versión con cola de salida a la primera.** Orden recomendado:

1. **(10 min)** Copia `extract_message` + `str_join` del `main.c` que te dan. Tal cual.
2. **(15 min)** Escribe el `main`: args → socket → bind → listen → `select` loop con
   `accept` y `recv`. Compila. Que acepte conexiones.
3. **(10 min)** Añade `ids`, `msgs`, `notify_other` con `send()` directo (versión simple).
   En este punto ya pasas la mayoría del tester.
4. **(15 min)** Si te sobra tiempo: mete `to_send` + `flush_client` para los clientes lazy.
5. **(5 min)** Aplica el fix del `write_fds` selectivo (bug 1) y el checklist de §7.

**Lo que más gente falla:**
- Olvidar el `exit(1)` tras "Wrong number of arguments".
- Enviar el mensaje también al autor.
- Poner el prefijo una vez por `recv` en vez de una vez por línea.
- Reutilizar el ID al reconectar (usar `fd` como id en vez de un contador global).
- No hacer `read_fds = all_fds` dentro del while (`select` destruye los sets).
- `select(max_fd, ...)` en vez de `max_fd + 1`.
- Dejar un `printf` de debug → función prohibida → 0.

**Los tres números que hay que memorizar:**
`"Wrong number of arguments\n"` → 26 · `"Fatal error\n"` → 12 · `127.0.0.1` → 2130706433
