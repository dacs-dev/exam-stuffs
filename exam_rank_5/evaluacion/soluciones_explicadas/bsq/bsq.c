/*
** ============================================================
**  bsq.c  —  Núcleo de la solución: parseo, validación y algoritmo DP
** ============================================================
**
**  VISIÓN GENERAL DEL FLUJO
**  ────────────────────────
**  1. parse_map()  lee el fichero y construye el t_map en memoria.
**     a) parse_header() analiza la primera línea: "9 . o x"
**        (número de filas + los tres caracteres especiales).
**     b) Por cada fila del mapa:
**        - read_next_row()       llama a getline para obtener la línea.
**        - strip_nl_get_len()    elimina el '\n' final y devuelve la longitud.
**        - valid_line_chars()    comprueba que solo haya 'empty' u 'obstacle'.
**  2. solve()  aplica programación dinámica para encontrar el cuadrado mayor.
**  3. fill_best_square()  sobrescribe las celdas del cuadrado con 'full'.
**  4. print_map() / free_map() → imprimir y liberar.
**
**  GESTIÓN DE ERRORES
**  ──────────────────
**  Cualquier violación de las reglas del mapa desemboca en err():
**    - Llama a free_map() para liberar lo ya asignado (sin fugas).
**    - Imprime "map error\n" en STDERR (no en stdout).
**    - La función que detectó el error devuelve 0 → main salta al siguiente mapa.
**
**  ALGORITMO DE PROGRAMACIÓN DINÁMICA (solve)
**  ───────────────────────────────────────────
**  Se crea una tabla dp_table[rows][cols] de enteros.
**  dp_table[i][j] = tamaño del mayor cuadrado cuya esquina INFERIOR-DERECHA
**                   es la celda (i, j).
**
**  Recurrencia:
**    si grid[i][j] == obstacle  → dp[i][j] = 0
**    si i==0 o j==0             → dp[i][j] = 1  (borde del mapa)
**    en otro caso               → dp[i][j] = 1 + min(dp[i-1][j],
**                                                     dp[i][j-1],
**                                                     dp[i-1][j-1])
**
**  Intuición del min():
**    Para que exista un cuadrado de tamaño k con esquina en (i,j) necesitamos:
**      - Un cuadrado de tamaño k-1 terminando en (i-1,j)   ← "arriba"
**      - Un cuadrado de tamaño k-1 terminando en (i,j-1)   ← "izquierda"
**      - Un cuadrado de tamaño k-1 terminando en (i-1,j-1) ← "diagonal"
**    El cuello de botella es el mínimo de los tres.
**
**  Desempate (más arriba y más a la izquierda):
**    Recorremos el mapa fila a fila, columna a columna (orden natural).
**    Solo actualizamos best cuando dp[i][j] > best_size (estrictamente mayor),
**    por lo que la PRIMERA posición que alcanza el máximo gana el desempate.
**
**  MEMORIA 2D
**  ──────────
**  grid   → calloc(rows+1, sizeof(char*))  (+1 para guardia NULL)
**  grid[i]→ devuelto por getline (ya alojado con malloc internamente)
**  dp_table → alloc_dp_table: array de punteros + un calloc por fila
**           → liberado por free_dp_table antes de salir de solve()
** ============================================================
*/

#include "bsq.h"

/* ── UTILIDADES ──────────────────────────────────────────── */

/*
** print_map: recorre grid fila a fila e imprime cada cadena seguida de '\n'.
** Se llama DESPUÉS de solve(), cuando el cuadrado ya está marcado con 'full'.
*/
void	print_map(t_map *map)
{
	for (int i = 0; i < map->rows; i++)
		fprintf(stdout, "%s\n", map->grid[i]);
}

/*
** err: manejador centralizado de errores de mapa.
**   1. Libera la memoria ya asignada en map->grid (evita fugas).
**   2. Imprime "map error\n" en STDERR (requerido por el enunciado).
** Se llama siempre antes de devolver 0 desde parse_map.
*/
static void	err(t_map *map)
{
	free_map(map);
	fprintf(stderr, "map error\n");
}

/*
** free_map: libera el array bidimensional grid.
**   - Libera cada fila (cadena) individualmente.
**   - Libera el array de punteros.
**   - Pone grid=NULL y rows=0 para evitar double-free accidentales.
*/
void	free_map(t_map *map)
{
	for (int i = 0; i < map->rows; i++)
		free(map->grid[i]);
	free(map->grid);
	map->grid = NULL;
	map->rows = 0;
}

/* ── PARSEO ──────────────────────────────────────────────── */

/*
** parse_header: lee e interpreta la primera línea del fichero.
**
** Formato esperado (según el enunciado):
**   "9 . o x\n"   →  rows=9, empty='.', obstacle='o', full='x'
**
** NOTA (aviso del subject): en la versión del examen real los tres
** caracteres pueden aparecer SIN espacio separador.  Esta implementación
** los lee directamente como cursor[0], cursor[1], cursor[2] justo
** después de los dígitos, sea cual sea el separador.
**
** Validaciones:
**   - rows > 0              (al menos una fila)
**   - Los tres caracteres son distintos entre sí
**   - Existen los tres caracteres (la cadena tiene al menos 3 chars tras los dígitos)
**
** Devuelve 0 si la cabecera es inválida, 1 si es correcta.
** En ambos casos libera 'line' (getline la aloja con malloc).
*/
static int	parse_header(t_map *map, FILE *file)
{
	char	*line = NULL;
	size_t	buf_size = 0;
	char	*cursor;

	/* getline devuelve -1 en EOF o error; <= 0 cubre ambos */
	if (getline(&line, &buf_size, file) <= 0)
		return (free(line), 1);  /* fichero vacío o error → fallo */

	/* Convertimos los dígitos iniciales al número de filas */
	map->rows = ft_atoi(line);

	/* Avanzamos cursor hasta el primer carácter NO dígito */
	cursor = line;
	while (*cursor >= '0' && *cursor <= '9')
		cursor++;

	/* Necesitamos al menos 3 caracteres tras los dígitos */
	if (!cursor[0] || !cursor[1] || !cursor[2])
		return (free(line), 0);

	/* Leemos los tres caracteres especiales */
	map->empty    = cursor[0];
	map->obstacle = cursor[1];
	map->full     = cursor[2];

	/* Validación: filas positivas y tres chars distintos */
	if (map->rows <= 0
		|| map->empty == map->obstacle
		|| map->full == map->empty
		|| map->obstacle == map->full)
		return (free(line), 0);

	return (free(line), 1);
}

/*
** valid_line_chars: verifica que cada carácter de la línea sea
** 'empty' u 'obstacle'.  Cualquier otro carácter invalida el mapa.
** Devuelve 1 si la línea es válida, 0 si contiene un carácter extraño.
*/
static int	valid_line_chars(t_map *map, char *line, int length)
{
	int	col = 0;

	while (col < length)
	{
		if (line[col] != map->empty && line[col] != map->obstacle)
			return (0);
		col++;
	}
	return (1);
}

/*
** strip_nl_get_len: elimina el '\n' final de la línea (si existe)
** y devuelve la longitud resultante sin el terminador de línea.
** Necesario porque getline incluye el '\n' en el buffer.
*/
static int	strip_nl_get_len(char *line)
{
	int	len = 0;

	while (line[len] && line[len] != '\n')
		len++;
	line[len] = '\0';   /* sustituimos '\n' por '\0' (fin de cadena) */
	return (len);
}

/*
** read_next_row: llama a getline para leer la siguiente fila del fichero.
** Devuelve el puntero a la línea alojada (responsabilidad del llamador
** liberarla o guardarla en grid), o NULL si se agotó el fichero / error.
** En caso de fallo libera el buffer que getline pudo haber creado.
*/
static char	*read_next_row(FILE *file)
{
	char	*line = NULL;
	size_t	buf_size = 0;

	if (getline(&line, &buf_size, file) <= 0)
		return (free(line), NULL);
	return (line);
}

/*
** parse_map: orquesta la lectura completa del mapa desde 'file'.
**
** Pasos:
**  1. Inicializa map (grid=NULL, rows=0, cols=-1 como "no determinado aún").
**  2. parse_header() → rellena rows, empty, obstacle, full.
**  3. calloc(rows+1) → reserva el array de punteros a fila (+1 guardia NULL).
**  4. Bucle fila a fila:
**     a. Leer línea con read_next_row.
**     b. Quitar '\n' y obtener longitud real.
**     c. La primera fila fija map->cols; las siguientes deben coincidir.
**     d. Validar que la longitud no sea 0, que coincida con cols anterior
**        y que solo contenga los caracteres permitidos.
**     e. Guardar el puntero en grid[row_index++].
**  5. Comprobación final: se leyeron exactamente rows filas.
**
** Devuelve 1 si todo fue bien, 0 si algo falló (err() ya habrá
** imprimido "map error" y liberado la memoria).
*/
int	parse_map(t_map *map, FILE *file)
{
	char	*line;
	int		row_index = 0;
	int		line_len;

	map->grid = NULL;
	map->rows = 0;
	map->cols = -1;   /* -1 = "todavía no conocemos las columnas" */

	if (!parse_header(map, file))
		return (err(map), 0);

	/* Reservamos el array de punteros (una entrada extra como guardia NULL) */
	if (!(map->grid = calloc(map->rows + 1, sizeof(char *))))
		return (err(map), 0);

	while (row_index < map->rows)
	{
		line = read_next_row(file);
		if (!line)
			return (err(map), 0);   /* Menos filas de las declaradas → error */

		line_len = strip_nl_get_len(line);

		/* La primera fila establece el ancho; las demás deben igualarlo */
		if (map->cols == -1)
			map->cols = line_len;

		/* Validaciones: longitud coherente y solo chars permitidos */
		if (line_len == 0 || line_len != map->cols
			|| !valid_line_chars(map, line, line_len))
			return (err(map), 0);

		map->grid[row_index++] = line;   /* Guardamos el puntero en el grid */
	}

	/* Doble comprobación: ¿leímos exactamente las filas declaradas? */
	if (row_index != map->rows)
		return (err(map), 0);

	return (1);
}

/* ── ALGORITMO DE PROGRAMACIÓN DINÁMICA ──────────────────── */

/*
** min3: devuelve el mínimo de tres enteros.
** Usado en la recurrencia de la tabla DP.
*/
static int	min3(int a, int b, int c)
{
	return (a < b ? (a < c ? a : c) : (b < c ? b : c));
}

/*
** alloc_dp_table: crea la tabla 2D de enteros para el algoritmo DP.
** Cada dp_table[row] es un array de 'num_cols' ints inicializados a 0.
** Devuelve NULL si algún malloc falla (sin liberar lo ya asignado —
** en este contexto el programa termina de todos modos).
*/
static int	**alloc_dp_table(int num_rows, int num_cols)
{
	int	**dp_table;

	if (!(dp_table = calloc(num_rows, sizeof(int *))))
		return (NULL);
	for (int row = 0; row < num_rows; row++)
	{
		if (!(dp_table[row] = calloc(num_cols, sizeof(int))))
			return (NULL);
	}
	return (dp_table);
}

/*
** free_dp_table: libera la tabla DP fila a fila, luego el array de punteros.
** Se llama siempre al final de solve() para no dejar fugas.
*/
static void	free_dp_table(int **dp_table, int num_rows)
{
	for (int row = 0; row < num_rows; row++)
		free(dp_table[row]);
	free(dp_table);
}

/*
** square_size_at: calcula dp[row][col], es decir, el tamaño del mayor
** cuadrado cuya esquina inferior-derecha está en (row, col).
**
** Casos base:
**   - Si es obstáculo → 0 (ningún cuadrado puede terminar aquí).
**   - Si es borde (fila 0 o columna 0) → 1 (cuadrado 1×1).
**
** Caso general:
**   dp[row][col] = 1 + min(arriba, izquierda, diagonal)
**
** La intuición: para que quepa un cuadrado de lado k con esquina en (row,col),
** los tres vecinos ya calculados deben poder "sostener" un cuadrado de lado k-1.
** El cuello de botella (min) determina el lado máximo posible.
*/
static int	square_size_at(int **dp_table, t_map *map, int row, int col)
{
	if (map->grid[row][col] == map->obstacle)
		return (0);
	if (row == 0 || col == 0)
		return (1);
	return (min3(dp_table[row - 1][col],    /* arriba    */
			dp_table[row][col - 1],          /* izquierda */
			dp_table[row - 1][col - 1]) + 1); /* diagonal  */
}

/*
** fill_best_square: rellena con el carácter 'full' las celdas del cuadrado
** ganador.  Recibe la esquina INFERIOR-DERECHA (bottom_row, right_col)
** y el tamaño 'size'.
**
** La esquina SUPERIOR-IZQUIERDA es:
**   fila:    bottom_row - size + 1
**   columna: right_col  - size + 1
**
** Recorre esas filas y columnas y sobreescribe con map->full.
** (No necesita liberar nada; modifica las cadenas ya en memoria.)
*/
static void	fill_best_square(t_map *map, int bottom_row, int right_col, int size)
{
	int	row;
	int	col;

	row = bottom_row - size + 1;  /* Esquina superior del cuadrado */
	while (row <= bottom_row)
	{
		col = right_col - size + 1;   /* Columna izquierda del cuadrado */
		while (col <= right_col)
			map->grid[row][col++] = map->full;
		row++;
	}
}

/*
** solve: función principal del algoritmo.
**
** 1. Aloja la tabla DP.
** 2. Recorre el mapa en orden fila→columna (arriba-izquierda a abajo-derecha).
**    Para cada celda calcula dp[i][j] con square_size_at().
**    Si dp[i][j] > best_size (estrictamente mayor), actualiza el mejor.
**    → El recorrido en orden natural garantiza el desempate:
**      la primera celda que alcance el máximo es la más arriba y a la izquierda.
** 3. fill_best_square() pinta el cuadrado en el grid original.
** 4. free_dp_table() libera la tabla DP.
**
** Si alloc_dp_table falla, la función retorna sin hacer nada
** (el mapa se imprimirá sin modificar).
*/
void	solve(t_map *map)
{
	int		**dp_table;
	int		best_size;
	int		best_bottom_row;
	int		best_right_col;
	int		row;
	int		col;

	if (!(dp_table = alloc_dp_table(map->rows, map->cols)))
		return ;

	best_size = 0;
	best_bottom_row = 0;
	best_right_col = 0;
	row = 0;
	while (row < map->rows)
	{
		col = 0;
		while (col < map->cols)
		{
			dp_table[row][col] = square_size_at(dp_table, map, row, col);
			/* Sólo actualizamos con > (no >=) para conservar el desempate
			   "más arriba y más a la izquierda" que impone el enunciado. */
			if (dp_table[row][col] > best_size)
			{
				best_size = dp_table[row][col];
				best_bottom_row = row;
				best_right_col = col;
			}
			col++;
		}
		row++;
	}
	fill_best_square(map, best_bottom_row, best_right_col, best_size);
	free_dp_table(dp_table, map->rows);
}
