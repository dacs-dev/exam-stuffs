# `mini_serv` — Paso a paso, de arriba a abajo

> Escribe el archivo **en este orden**, de la línea 1 a la última. Cada paso dice si es
> **[COPIAR]** del `main.c` que te dan o **[ESCRIBIR]** de memoria.
> Al final tienes `mini_serv_minimo.c`: 161 líneas, probado con `-Wall -Wextra -Werror` y `nc`.

---

## ⚠️ Antes de nada: los 6 fallos de la trampa

Estos son los que se cuelan al copiar del `main.c`, porque el `main.c` **está escrito para
romper el subject a propósito**:

| Trampa del `main.c` | Lo correcto |
|---|---|
| `printf("...")` por todos lados | **prohibida** — bórralos todos, sin excepción |
| `exit(0)` en los errores | debe ser **`exit(1)`** |
| `htons(8081)` | `htons(atoi(argv[1]))` — ¡`argv[1]` es `char*`, no int! |
| `accept(...)` suelto al final | va **dentro** del bucle `select` |
| `int sockfd;` local a `main` | **global**, o `send_all` no lo ve |
| contar mal los bytes del `write` | `"Fatal error\n"` = **12** · `"Wrong number of arguments\n"` = **26** |

> Los bytes se cuentan **con el `\n` y sin el `\0`**. Si pones 13 y 28 el programa "funciona"
> pero escupe bytes de basura al stderr. Verificado: son 12 y 26.

---

## PASO 1 — Includes  `[COPIAR]`

```c
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>          // sprintf vive aqui, y SI esta permitida
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>     // ← ESTE NO ESTA EN main.c, lo añades tu (select, fd_set)
```

`errno.h` y `netdb.h` los puedes dejar o quitar, da igual.

---

## PASO 2 — `extract_message` y `str_join`  `[COPIAR]`

Cópialas del `main.c` **literalmente**. No las leas, no las mejores, no las toques.
Son 44 líneas gratis.

---

## PASO 3 — Variables globales  `[ESCRIBIR]` ← no están en `main.c`

```c
int     ids[1024];          // ids[fd] = id logico del cliente que ocupa ese fd
char    *msgs[1024];        // buffer de entrada por fd (lo recibido sin \n final)
int     count = 0;          // siguiente id a repartir. SOLO SUBE
int     max_fd = 0;         // fd mas alto vivo
int     sockfd;             // socket de escucha  ← GLOBAL, no dentro de main
fd_set  afds, rfds, wfds;
char    buf_read[1001], buf_write[64];
```

**¿Por qué globales?** Porque `send_all()` necesita `max_fd`, `sockfd` y `wfds`. Si los
declaras dentro de `main`, `send_all` no compila. Globales es gratis y son 0 por defecto.

**¿Por qué arrays indexados por fd?** El kernel siempre te da el fd libre más bajo, así que
`ids[fd]` y `msgs[fd]` son un diccionario O(1) gratis. Nada de listas enlazadas.

---

## PASO 4 — `fatal()`  `[ESCRIBIR]`

```c
void fatal(void)
{
    write(2, "Fatal error\n", 12);   // ← 12, contados
    exit(1);                          // ← 1, no 0
}
```

---

## PASO 5 — `send_all()`  `[ESCRIBIR]` ← no está en `main.c`

```c
void send_all(int except, char *s)
{
    for (int i = 0; i <= max_fd; i++)
        if (FD_ISSET(i, &wfds) && i != except && i != sockfd)
            send(i, s, strlen(s), 0);
}
```

Recorre todos los fds y manda a todos **menos**: el autor (`except`, que no debe verse a sí
mismo) y el socket de escucha (`sockfd`, que no es un cliente).

---

## PASO 6 — Cabecera del `main`  `[ESCRIBIR]`

```c
int main(int argc, char **argv)
{
    struct sockaddr_in servaddr;

    if (argc != 2)
    {
        write(2, "Wrong number of arguments\n", 26);   // ← 26, contados
        exit(1);                                        // ← EL OLVIDO MAS COMUN
    }
```

Sin ese `exit(1)` el programa sigue, llega a `atoi(argv[1])` con `argv[1] == NULL` y
**segfaultea**.

---

## PASO 7 — Crear el socket  `[COPIAR con cambios]`

En `main.c` es esto (con `printf` y `exit(0)`):

```c
sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd == -1) { printf("socket creation failed...\n"); exit(0); }
else printf("Socket successfully created..\n");
```

Tú escribes:

```c
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        fatal();
    max_fd = sockfd;          // ← NO esta en main.c
    FD_ZERO(&afds);           // ← NO esta en main.c
    FD_SET(sockfd, &afds);    // ← NO esta en main.c
```

### Qué son `FD_ZERO` y `FD_SET` (esto no lo explica el subject)

Un `fd_set` es una **lista de casillas**, una por cada fd posible. `select` no sabe qué fds
te interesan: se lo tienes que decir marcando casillas.

| Macro | Qué hace | Analogía |
|---|---|---|
| `FD_ZERO(&s)` | desmarca **todas** las casillas | vaciar la lista |
| `FD_SET(fd, &s)` | marca la casilla de `fd` | apuntar a `fd` en la lista |
| `FD_CLR(fd, &s)` | desmarca la de `fd` | tachar a `fd` de la lista |
| `FD_ISSET(fd, &s)` | ¿está marcada? | ¿está `fd` en la lista? |

**`FD_ZERO` primero, siempre.** Un `fd_set` sin inicializar contiene basura de la pila y
`select` se volvería loco. Es el `bzero` de los `fd_set`.

Y **`FD_SET(sockfd, &afds)`** mete el socket de escucha en la lista para que `select` te
avise *"hay alguien llamando a la puerta"*. Sin esta línea nunca te enteras de las
conexiones entrantes y el servidor no acepta a nadie.

Los tres sets:

- **`afds`** = *la verdad*: todos los fds vivos (escucha + clientes). La mantienes tú con
  `FD_SET`/`FD_CLR`.
- **`rfds` / `wfds`** = copias de usar y tirar. **`select` las machaca**: al volver solo
  quedan marcados los que están listos. Por eso hay que rehacerlas en **cada vuelta**.

---

## PASO 8 — bind + listen  `[COPIAR con cambios]`

```c
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);   // main.c: htonl(2130706433)
    servaddr.sin_port = htons(atoi(argv[1]));            // main.c: htons(8081)

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
        fatal();
    if (listen(sockfd, 128) != 0)
        fatal();
```

⚠️ **`atoi(argv[1])`**, no `argv[1]` a secas. `htons` espera un entero; pasarle el `char*`
es un `-Wint-conversion` y un puerto aleatorio.

`htonl(INADDR_LOOPBACK)` == `htonl(2130706433)` == `htonl(0x7F000001)`, verificado. Usa
`INADDR_LOOPBACK` y no memorizas nada.

---

## PASO 9 — El bucle  `[ESCRIBIR]` ← el núcleo, lo único que hay que entender

```c
    while (1)
    {
        rfds = wfds = afds;                                     // (1) rehacer las copias
        if (select(max_fd + 1, &rfds, &wfds, NULL, NULL) < 0)   // (2) max_fd + 1
            fatal();

        for (int fd = 0; fd <= max_fd; fd++)                    // (3) ¿quien esta listo?
        {
            if (!FD_ISSET(fd, &rfds))
                continue;
```

**(1)** dentro del `while`, no fuera: `select` destruye los sets.
**(2)** `max_fd + 1` = *cuántos* fds mirar, no el índice máximo.
**(3)** `select` te dice *cuántos* están listos, no *cuáles* → hay que recorrer y preguntar.

### Caso A — llega alguien

```c
            if (fd == sockfd)
            {
                int cfd = accept(sockfd, NULL, NULL);
                if (cfd < 0)
                    continue;
                if (cfd > max_fd)
                    max_fd = cfd;
                ids[cfd] = count++;        // ← contador global, NUNCA el fd
                msgs[cfd] = NULL;          // ← el fd pudo ser de otro cliente antes
                FD_SET(cfd, &afds);        // ← darlo de alta en la lista
                sprintf(buf_write, "server: client %d just arrived\n", ids[cfd]);
                send_all(cfd, buf_write);
                break;                     // afds y max_fd cambiaron → rehacer select
            }
```

### Caso B — se va alguien

```c
            int r = recv(fd, buf_read, 1000, 0);
            if (r <= 0)
            {
                sprintf(buf_write, "server: client %d just left\n", ids[fd]);
                send_all(fd, buf_write);   // avisar ANTES de cerrar
                free(msgs[fd]);
                msgs[fd] = NULL;
                FD_CLR(fd, &afds);         // ← darlo de baja
                close(fd);                 // ← sin esto: fuga de fds
                break;
            }
```

### Caso C — habla alguien

```c
            buf_read[r] = 0;                            // buf[1001], recv 1000 → cabe el \0
            msgs[fd] = str_join(msgs[fd], buf_read);    // acumular: la linea puede venir partida
            if (msgs[fd] == 0)
                fatal();

            char *msg;
            int ret;
            while ((ret = extract_message(&msgs[fd], &msg)) > 0)
            {
                sprintf(buf_write, "client %d: ", ids[fd]);
                send_all(fd, buf_write);    // 1) prefijo, DENTRO del while → uno POR LINEA
                send_all(fd, msg);          // 2) la linea (ya trae su \n)
                free(msg);
            }
            if (ret < 0)
                fatal();
        }
    }
    return 0;
}
```

⚠️ `> 0`, no solo `while (extract_message(...))`. Si devuelve `-1` (falló el `calloc`), `-1`
es **verdadero** → el buffer no cambia → devuelve `-1` otra vez → **bucle infinito**.

**Y ya está. El programa está entero.**

---

## Lo único que hay que memorizar (7 cosas)

```
1.  "Wrong number of arguments\n" → 26      + exit(1)
2.  "Fatal error\n"               → 12      + exit(1)
3.  FD_ZERO(&afds); FD_SET(sockfd, &afds);  ← justo tras crear el socket
4.  rfds = wfds = afds;   DENTRO del while  ← select destruye los sets
5.  select(max_fd + 1, ...)                 ← el +1
6.  ids[cfd] = count++;                     ← contador global, jamas el fd
7.  htons(atoi(argv[1]))                    ← atoi, no argv[1] pelado
```

Los 3 formatos NO los memorices, los tienes en el enunciado delante:
`"server: client %d just arrived\n"` · `"server: client %d just left\n"` · `"client %d: "`

---

## Test de 60 segundos antes de entregar

```bash
gcc -Wall -Wextra -Werror mini_serv.c   # ni un solo warning
grep -n printf mini_serv.c              # VACIO (funcion prohibida)
grep -n define mini_serv.c              # VACIO
./a.out ; echo $?                       # "Wrong number of arguments" y un 1
./a.out 8080 &
nc 127.0.0.1 8080     # terminal 2
nc 127.0.0.1 8080     # terminal 3
```

1. Al conectar el 2º, el 1º ve `server: client 1 just arrived`
2. El 2º escribe → el 1º lo ve, **el 2º NO se ve a sí mismo**
3. `printf 'a\nb\nc\n' | nc 127.0.0.1 8080` → 3 líneas, **cada una con su prefijo**
4. Escribe sin Enter → no sale nada hasta el `\n` ✔
5. Ctrl-C en el 2º → el 1º ve `server: client 1 just left`
6. Reconecta → recibe id **2**, no 1 ← *los ids no se reciclan*

---

## Nota honesta

Esta versión manda con `send()` directo, así que un cliente que no lee puede perder
mensajes, y gasta ~60% de CPU en vacío (porque `wfds = afds` y un socket sin backlog
siempre es escribible → `select` retorna al instante). **El corrector no mide CPU y esta
versión pasa.**

⚠️ No intentes arreglar la CPU aquí: el fix es poner en `wfds` solo los fds con datos
pendientes, pero como `send_all` comprueba `FD_ISSET(i, &wfds)` y aquí no hay cola de
salida, `wfds` quedaría vacío y **no se enviaría nada nunca**. O dejas `wfds = afds`
(esta versión), o montas la cola entera → `mini_serv_fixed.c`.
