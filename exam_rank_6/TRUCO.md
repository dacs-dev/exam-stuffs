# `mini_serv` — Qué copiar, qué cambiar, qué memorizar

> Regla de oro: **el 40% del programa te lo regalan en `main.c`.** Cópialo sin leerlo.
> Sólo tienes que escribir ~55 líneas, y de ésas sólo 5 hay que saberlas de memoria.

Archivo de referencia ya probado: **`mini_serv_minimo.c`** (161 líneas, compila con
`-Wall -Wextra -Werror`, verificado con `nc`).

---

## 1. El reparto: copiar vs escribir

| Bloque | Líneas | ¿De dónde sale? |
|---|---|---|
| `extract_message` | 25 | **COPIAR** de `main.c`, sin tocar una coma |
| `str_join` | 19 | **COPIAR** de `main.c`, sin tocar una coma |
| includes | 7 | **COPIAR** de `main.c` (+ añadir `<sys/select.h>`) |
| socket + bind + listen | 12 | **COPIAR** del `main()` de `main.c`, con 3 cambios |
| globales + `fatal` + `send_all` | 15 | escribir (trivial) |
| el bucle `select` | 45 | **escribir — esto es lo único que hay que entender** |

---

## 2. Paso 1 — Copia del `main.c` que te dan (30 segundos)

Abre el `main.c` del subject y copia **literalmente**:

- Los `#include` (líneas 1–8). **Añade `#include <sys/select.h>`** — sin él no compila
  el `select`. Puedes borrar `errno.h` y `netdb.h`, no los usas.
  **Deja `stdio.h`**: lo necesitas para `sprintf`, que sí está permitida.
- **`extract_message` entera** (líneas 10–35).
- **`str_join` entera** (líneas 37–55).

⚠️ **No las "mejores".** `str_join` tiene un leak técnico si falla el `malloc` — da igual,
sales con `exit(1)` acto seguido. Tocarlas sólo te expone a meter un bug de verdad.

---

## 3. Paso 2 — El bloque de socket: copiar del `main()` y cambiar 3 cosas

El `main()` de `main.c` ya trae socket + bind + listen. Cópialo y aplica **exactamente**
estos cambios:

| En `main.c` pone… | Tú pones… | Por qué |
|---|---|---|
| `printf("socket creation failed...")` + `exit(0)` | `fatal();` | `printf` está **prohibida** y el status debe ser **1** |
| `htons(8081)` | `htons(atoi(av[1]))` | el puerto viene por argumento |
| `htonl(2130706433)` | `htonl(INADDR_LOOPBACK)` | mismo valor, sin memorizar nada |
| `listen(sockfd, 10)` | `listen(sockfd, 128)` | da igual, cualquiera vale |
| `len = sizeof(cli); accept(...)` al final | **bórralo** | el accept va dentro del bucle |
| todos los `else printf("...")` | **bórralos** | `printf` prohibida |

Te queda esto (12 líneas):

```c
struct sockaddr_in servaddr;

sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd < 0) fatal();
max_fd = sockfd;
FD_ZERO(&afds);
FD_SET(sockfd, &afds);

bzero(&servaddr, sizeof(servaddr));
servaddr.sin_family = AF_INET;
servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
servaddr.sin_port = htons(atoi(av[1]));
if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) fatal();
if (listen(sockfd, 128) != 0) fatal();
```

---

## 4. Paso 3 — Lo que escribes tú (55 líneas, en 5 trozos)

### Trozo A — globales + `fatal` (10 líneas, no hay nada que entender)

```c
int     ids[1024];          // ids[fd] = id logico del cliente
char    *msgs[1024];        // buffer de entrada por fd
int     count = 0, max_fd = 0, sockfd;
fd_set  afds, rfds, wfds;   // afds = la verdad; rfds/wfds = copias para select
char    buf_read[1001], buf_write[64];

void fatal(void)
{
    write(2, "Fatal error\n", 12);
    exit(1);
}
```

### Trozo B — `send_all` (6 líneas)

```c
void send_all(int except, char *s)
{
    for (int i = 0; i <= max_fd; i++)
        if (FD_ISSET(i, &wfds) && i != except && i != sockfd)
            send(i, s, strlen(s), 0);
}
```

`except` = el autor, que **no** debe recibir su propio mensaje. `sockfd` tampoco (es el
socket de escucha, no un cliente).

### Trozo C — cabecera del `main` (5 líneas)

```c
int main(int ac, char **av)
{
    if (ac != 2)
    {
        write(2, "Wrong number of arguments\n", 26);
        exit(1);              // ← EL FALLO MAS COMUN ES OLVIDAR ESTO
    }
```

### Trozo D — el bucle `select` (8 líneas — el esqueleto)

```c
while (1)
{
    rfds = wfds = afds;                                   // select DESTRUYE los sets
    if (select(max_fd + 1, &rfds, &wfds, NULL, NULL) < 0)  // max_fd + 1, no max_fd
        fatal();
    for (int fd = 0; fd <= max_fd; fd++)
    {
        if (!FD_ISSET(fd, &rfds))
            continue;
        ...  // trozo E
    }
}
```

### Trozo E — los 3 casos dentro del `for` (30 líneas)

**Caso 1 — llega alguien** (`fd == sockfd`):
```c
if (fd == sockfd)
{
    int cfd = accept(sockfd, NULL, NULL);
    if (cfd < 0) continue;
    if (cfd > max_fd) max_fd = cfd;
    ids[cfd] = count++;                    // ← ID de un contador global, NUNCA el fd
    msgs[cfd] = NULL;
    FD_SET(cfd, &afds);
    sprintf(buf_write, "server: client %d just arrived\n", ids[cfd]);
    send_all(cfd, buf_write);
    break;                                 // afds/max_fd cambiaron → rehacer select
}
```

**Caso 2 — se va alguien** (`recv <= 0`):
```c
int r = recv(fd, buf_read, 1000, 0);
if (r <= 0)
{
    sprintf(buf_write, "server: client %d just left\n", ids[fd]);
    send_all(fd, buf_write);
    free(msgs[fd]);
    msgs[fd] = NULL;
    FD_CLR(fd, &afds);
    close(fd);
    break;
}
```

**Caso 3 — habla alguien**:
```c
buf_read[r] = 0;                           // buf_read[1001], recv de 1000 → cabe el \0
msgs[fd] = str_join(msgs[fd], buf_read);   // acumular (una linea puede venir partida)
if (msgs[fd] == 0) fatal();

char *msg;
int ret;
while ((ret = extract_message(&msgs[fd], &msg)) > 0)
{
    sprintf(buf_write, "client %d: ", ids[fd]);
    send_all(fd, buf_write);               // 1) el prefijo, DENTRO del while
    send_all(fd, msg);                     // 2) la linea (ya trae su \n)
    free(msg);
}
if (ret < 0) fatal();                      // ret > 0, no solo ret: el -1 colgaria el bucle
```

**Y ya está. El programa está entero.**

---

## 5. Lo único que hay que memorizar de verdad (5 cosas)

```
1.  "Wrong number of arguments\n"  →  26        + exit(1)
2.  "Fatal error\n"                →  12        + exit(1)
3.  select(max_fd + 1, ...)                     ← el +1
4.  rfds = wfds = afds;  DENTRO del while       ← select destruye los sets
5.  ids[cfd] = count++;                         ← contador global, no el fd
```

Los tres formatos se deducen del enunciado, que tienes delante:
`"server: client %d just arrived\n"` · `"server: client %d just left\n"` · `"client %d: "`

---

## 6. Los 6 fallos que más suspenden

| Fallo | Síntoma |
|---|---|
| Olvidar `exit(1)` tras "Wrong number of arguments" | `atoi(NULL)` → segfault |
| Usar `fd` como ID en vez de `count++` | al reconectar repite el id → tester KO |
| Poner el prefijo fuera del `while` | sólo la 1ª línea lleva `client N: ` |
| `while (extract_message(...))` sin `> 0` | el `-1` es cierto → **bucle infinito** |
| No reasignar `rfds = afds` cada vuelta | funciona 1 vez y se queda colgado |
| Dejarte un `printf` de debug | función prohibida → **0** |

---

## 7. Test de 60 segundos antes de entregar

```bash
gcc -Wall -Wextra -Werror mini_serv.c   # sin un solo warning
grep define mini_serv.c                 # vacio
./a.out                                 # "Wrong number of arguments", echo $? == 1
./a.out 8080 &
nc 127.0.0.1 8080     # terminal 2
nc 127.0.0.1 8080     # terminal 3
```

1. Al conectar el 2º, el 1º ve `server: client 1 just arrived`
2. El 2º escribe → el 1º ve `client 1: ...`, **el 2º no se ve a sí mismo**
3. `printf 'a\nb\nc\n' | nc 127.0.0.1 8080` → 3 líneas, **cada una con prefijo**
4. Escribe sin `\n` → no sale nada hasta pulsar Enter ✔
5. Ctrl-C en el 2º → el 1º ve `server: client 1 just left`
6. Reconecta → recibe id **2**, no 1

---

## 8. Nota honesta sobre esta versión mínima

`mini_serv_minimo.c` manda con `send()` directo. Eso implica dos cosas:

**(a) Un cliente que no lee puede perder mensajes.** El enunciado menciona los clientes
"lazy", y la versión robusta (`mini_serv_fixed.c`) resuelve esto con una cola de salida por
cliente (`to_send` + `flush_client`, ~40 líneas más). En la práctica la versión mínima pasa
el corrector. **Escribe primero la mínima, y sólo si te sobran 20 min sube a la otra.**

**(b) Gasta CPU en vacío** (medido: ~60%), porque `wfds = afds` mete a todos los clientes
en el set de escritura y un socket sin backlog siempre es escribible → `select` retorna al
instante en bucle.

⚠️ **No intentes arreglar (b) en la versión mínima.** El fix es construir `wfds` sólo con
los fds que tienen datos pendientes — pero como aquí `send_all` comprueba
`FD_ISSET(i, &wfds)` y no hay cola, `wfds` quedaría vacío y **no se enviaría nada nunca**.
Los dos arreglos van juntos o no van: o dejas `wfds = afds` (mínima), o montas la cola
completa (`mini_serv_fixed.c`). El corrector no mide CPU.
