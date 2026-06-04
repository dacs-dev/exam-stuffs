/*
** ============================================================
** EJERCICIO: game_of_life  (Exam Rank 5 — Level 2)
** ============================================================
**
** USO:
**   ./life width height iterations
**
**   width      -> ancho del tablero (columnas)
**   height     -> alto del tablero (filas)
**   iterations -> número de pasos de la simulación
**
** FUNCIONES PERMITIDAS: atoi, read, putchar, malloc, calloc, realloc, free
** ¡OJO! printf NO está permitido → toda la salida se hace con putchar.
**
** DESCRIPCION GENERAL:
**   1. Parseamos los 3 argumentos con atoi(av[1..3]).
**   2. Leemos comandos de stdin con read() carácter a carácter:
**        w/a/s/d → mueven el lápiz (arriba/izquierda/abajo/derecha)
**        x       → alterna lápiz arriba/abajo (toggle dibujar)
**      - El lápiz empieza en (0,0) = esquina superior-izquierda.
**      - El lápiz NO sale del tablero: si un comando lo sacaría, se ignora.
**      - Cuando el lápiz está BAJADO ('x' activo), cada movimiento
**        (y también el hecho de estar en esa celda) marca la celda como VIVA.
**      - Comando inválido → se ignora.
**   3. Simulamos 'iterations' pasos del Juego de la Vida de Conway.
**   4. Imprimimos el estado final: celda viva = '0', muerta = ' ', '\n' por fila.
**
** REPRESENTACION DEL TABLERO (array 1D):
**   Usamos un array plano de width*height bytes (char).
**   La celda (x, y) está en la posición: board[y * width + x]
**   Valor 1 = viva, 0 = muerta.
**   Se usan DOS buffers (board y nextboard) para calcular la siguiente
**   generación sin sobreescribir el estado actual (ver función step).
**
** REGLAS DE CONWAY:
**   - Una celda VIVA con 2 ó 3 vecinos vivos → SOBREVIVE.
**   - Una celda VIVA con <2 ó >3 vecinos      → MUERE.
**   - Una celda MUERTA con exactamente 3 vecinos vivos → NACE.
**   - Los 8 vecinos son los adyacentes en diagonal y en cruz.
**   - Las celdas fuera del tablero cuentan como MUERTAS.
** ============================================================
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/*
** printboard — imprime el tablero en stdout usando putchar.
**
** Por cada fila (y) recorremos todas las columnas (x).
**   · Si la celda está viva (valor != 0) → imprimimos '0' (el carácter cero).
**   · Si está muerta              → imprimimos ' ' (espacio).
** Al acabar cada fila imprimimos '\n'.
**
** Nota: usamos putchar en lugar de printf porque el enunciado no
** permite printf.
*/
static void	printboard(char *board, int width, int height)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			/* board[y * width + x] vale 1 (viva) o 0 (muerta) */
			if (board[y * width + x])
				putchar('0');   /* celda viva → carácter '0' */
			else
				putchar(' ');   /* celda muerta → espacio */
		}
		putchar('\n');          /* salto de línea al terminar la fila */
	}
}

/*
** countneighbours — cuenta cuántos de los 8 vecinos de (x,y) están vivos.
**
** Recorre los offsets dy ∈ {-1,0,1} y dx ∈ {-1,0,1}, que generan las
** 9 posiciones del entorno 3×3. Saltamos (dx==0 && dy==0) que es la
** propia celda.
**
** Si el vecino cae fuera del tablero (neighbour_x < 0, etc.) simplemente
** no lo sumamos → las celdas fuera del tablero cuentan como muertas.
**
** Como board[] almacena 1 para vivo y 0 para muerto, basta con sumar
** el valor directamente: count += board[...].
*/
static int	countneighbours(char *board, int width, int height, int x, int y)
{
	int	count = 0;

	for (int dy = -1; dy <= 1; dy++)
	{
		for (int dx = -1; dx <= 1; dx++)
		{
			if (dx == 0 && dy == 0)
				continue ;              /* saltamos la celda central */

			int neighbour_x = x + dx;
			int neighbour_y = y + dy;

			/* solo contamos si el vecino está dentro del tablero */
			if ((neighbour_x >= 0 && neighbour_x < width) &&
				neighbour_y >= 0 && neighbour_y < height)
			{
				count += board[neighbour_y * width + neighbour_x];
			}
		}
	}
	return (count);
}

/*
** step — calcula UNA generación del Juego de la Vida.
**
** Recorre todas las celdas del tablero actual (board) y escribe el nuevo
** estado en nextboard. Es FUNDAMENTAL usar un buffer separado: si
** actualizásemos board directamente, las celdas ya modificadas afectarían
** al cálculo de sus vecinos en la misma generación, rompiendo las reglas.
**
** Reglas aplicadas:
**   · Celda VIVA con 2 ó 3 vecinos → sigue viva (nextboard = 1)
**   · Celda VIVA con otro número   → muere      (nextboard = 0)
**   · Celda MUERTA con 3 vecinos   → nace       (nextboard = 1)
**   · Celda MUERTA con otro número → sigue muerta (nextboard = 0)
*/
static void	step(char *board, char *nextboard, int width, int height)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			/* contamos vecinos vivos de la celda (x, y) */
			int n = countneighbours(board, width, height, x, y);

			if (board[y * width + x])
			{
				/* celda VIVA: sobrevive solo con 2 o 3 vecinos */
				if (n == 2 || n == 3)
					nextboard[y * width + x] = 1;
				else
					nextboard[y * width + x] = 0;
			}
			else
			{
				/* celda MUERTA: nace solo con exactamente 3 vecinos */
				if (n == 3)
					nextboard[y * width + x] = 1;
				else
					nextboard[y * width + x] = 0;
			}
		}
	}
}

/*
** main — punto de entrada del programa.
**
** Flujo:
**   1. Validar que recibimos exactamente 3 argumentos (ac == 4).
**   2. Convertir width, height e iter con atoi.
**   3. Reservar memoria para dos tableros con calloc (inicializa a 0 = todo muerto).
**   4. Leer stdin byte a byte con read() para posicionar el lápiz y dibujar.
**   5. Ejecutar 'iter' pasos de la simulación alternando los dos buffers.
**   6. Imprimir el tablero final y liberar la memoria.
*/
int main(int ac, char const *av[])
{
	/* necesitamos exactamente width, height, iterations */
	if (ac != 4)
		return (1);

	/* atoi convierte la cadena de texto del argumento a entero */
	int width  = atoi(av[1]);
	int height = atoi(av[2]);
	int iter   = atoi(av[3]);

	/*
	** calloc(n, size) reserva n*size bytes e inicializa todo a 0.
	** board     → estado actual del tablero
	** nextboard → buffer donde calculamos la siguiente generación
	** Ambos representan el tablero como array 1D: celda(x,y) = buf[y*width+x]
	*/
	char	*board     = calloc(width * height, 1);
	char	*nextboard = calloc(width * height, 1);

	if (!board || !nextboard)
		return (1);

	/*
	** Lápiz: comienza en (0,0) = esquina superior-izquierda.
	** pencil = 0 → lápiz arriba (no dibuja)
	** pencil = 1 → lápiz abajo  (dibuja al pasar/quedarse)
	*/
	int x = 0, y = 0, pencil = 0;
	char c;

	/*
	** Leemos un byte a la vez de stdin (fd=0).
	** read() devuelve el nº de bytes leídos; devuelve 0 al llegar a EOF.
	**
	** Comandos:
	**   'x' → alterna pencil (0→1 o 1→0) — sube/baja el lápiz
	**   'w' → sube  (y--)  solo si y > 0         (no sale por arriba)
	**   's' → baja  (y++)  solo si y < height-1   (no sale por abajo)
	**   'a' → izqda (x--)  solo si x > 0          (no sale por la izquierda)
	**   'd' → decha (x++)  solo si x < width-1     (no sale por la derecha)
	**   otros → ignorados
	**
	** Tras cada comando, si pencil==1 marcamos la celda actual como viva.
	** Así, bajar el lápiz con 'x' y luego moverse va marcando el camino.
	*/
	while (read(0, &c, 1) > 0)
	{
		if (c == 'x')
			pencil = !pencil;           /* toggle: alterna arriba/abajo */
		if (c == 'a' && x > 0)
			x--;                        /* izquierda, solo si no es el borde */
		else if (c == 'd' && x < width - 1)
			x++;                        /* derecha, solo si no es el borde */
		else if (c == 'w' && y > 0)
			y--;                        /* arriba, solo si no es el borde */
		else if (c == 's' && y < height - 1)
			y++;                        /* abajo, solo si no es el borde */

		/* si el lápiz está bajado, marcamos la celda actual como viva */
		if (pencil)
			board[y * width + x] = 1;
	}

	/*
	** Simulamos 'iter' generaciones.
	**
	** Cada llamada a step() escribe el nuevo estado en nextboard.
	** Después, intercambiamos los punteros (swap de punteros con tmp):
	**   - board     apunta al resultado recién calculado
	**   - nextboard apunta al buffer antiguo, listo para sobreescribirse
	** Así evitamos copiar memoria: solo intercambiamos punteros O(1).
	**
	** Si iter == 0, no ejecutamos ningún paso y se imprime el tablero inicial.
	*/
	for (int i = 0; i < iter; i++)
	{
		step(board, nextboard, width, height);

		/* swap de punteros para alternar roles entre board y nextboard */
		char *tmp  = board;
		board      = nextboard;
		nextboard  = tmp;
	}

	/* imprimimos el estado final del tablero */
	printboard(board, width, height);

	/* liberamos ambos buffers para evitar fugas de memoria */
	free(board);
	free(nextboard);
	return 0;
}
