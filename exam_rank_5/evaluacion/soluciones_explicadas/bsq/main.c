/*
** ============================================================
**  main.c  —  Punto de entrada y gestión de múltiples mapas
** ============================================================
**
**  RESPONSABILIDAD DE ESTE FICHERO
**  ────────────────────────────────
**  Determinar la fuente de datos (stdin o ficheros), abrir cada
**  fichero, orquestar el procesamiento de cada mapa y separar
**  las salidas con una línea en blanco cuando hay varios mapas.
**
**  MODOS DE USO
**  ────────────
**  Sin argumentos:   ./bsq
**    → Lee un único mapa desde stdin.
**
**  Con argumentos:   ./bsq mapa1.txt mapa2.txt …
**    → Procesa cada fichero en orden.
**    → Entre cada resultado (o "map error") imprime una línea en blanco,
**      EXCEPTO tras el último (is_last == 1).
**
**  FUNCIÓN process
**  ───────────────
**  Recibe el FILE* ya abierto y el flag is_last.
**  Flujo:
**    1. parse_map()  → si falla (devuelve 0), err() ya imprimió "map error"
**                      y liberó memoria; sólo añadimos la línea en blanco
**                      si no es el último mapa y retornamos.
**    2. solve()      → modifica el grid marcando el cuadrado mayor con 'full'.
**    3. print_map()  → vuelca el resultado en stdout.
**    4. free_map()   → libera la memoria del mapa.
**    5. Si no es el último, imprime '\n' de separación.
**
**  MANEJO DE ERROR AL ABRIR FICHERO
**  ──────────────────────────────────
**  Si fopen() devuelve NULL (fichero inexistente, sin permisos, etc.),
**  imprimimos "map error\n" en STDERR y, si no es el último argumento,
**  añadimos el '\n' separador en stdout (para mantener el formato).
**
**  NOTA SOBRE EL BUCLE i++ DOBLE
**  ──────────────────────────────
**  El bucle for ya incrementa i en su propia expresión; hay un i++ extra
**  al final del cuerpo, lo que hace que el bucle salte de dos en dos.
**  Esto es un bug de la solución original que se preserva tal cual
**  (no modificamos la lógica).
** ============================================================
*/

#include "bsq.h"

/*
** process: procesa un único mapa leído desde 'file'.
**
** @file    → stream ya abierto para lectura (stdin o fichero)
** @is_last → 1 si es el último mapa a procesar (no se añade '\n' al final)
**
** El flag is_last controla si se imprime la línea en blanco separadora.
** Cuando parse_map falla, err() ya escribió "map error\n" en stderr;
** aquí solo gestionamos el salto de línea en stdout si corresponde.
*/
static void	process(FILE *file, int is_last)
{
	t_map	map;

	if (!parse_map(&map, file))
	{
		/* parse_map ya llamó a err() → imprimió "map error" y liberó memoria */
		if (!is_last)
			fprintf(stdout, "\n");
		return ;   /* Pasamos al siguiente mapa sin llamar a solve/print */
	}
	solve(&map);      /* Aplica el algoritmo DP y marca el cuadrado mayor */
	print_map(&map);  /* Vuelca el resultado en stdout */
	free_map(&map);   /* Libera grid[] y sus filas */
	if (!is_last)
		fprintf(stdout, "\n");   /* Separador entre mapas consecutivos */
}

/*
** main: punto de entrada del programa.
**
** Sin argumentos (ac == 1):
**   Llama a process(stdin, 1) → un solo mapa desde la entrada estándar.
**
** Con argumentos:
**   Itera sobre av[1..ac-1].
**   Para cada argumento:
**     - Intenta abrir el fichero con fopen().
**     - Si falla → "map error\n" en stderr + '\n' separador si no es el último.
**     - Si tiene éxito → process() + fclose().
*/
int	main(int ac, char const *av[])
{
	FILE	*file;

	if (ac == 1)
		return (process(stdin, 1), 0);   /* Modo stdin: un solo mapa */

	for (int i = 1; i < ac; i++)
	{
		file = fopen(av[i], "r");
		if (!file)
		{
			/* No se pudo abrir el fichero → error de mapa */
			fprintf(stderr, "map error\n");
		}
		else
		{
			/* is_last = 1 cuando procesamos el último argumento */
			process(file, i == ac - 1);
			fclose(file);
		}
		/* Si el fichero no se abrió Y no es el último, añadimos separador */
		if (i < ac - 1 && !file)
			fprintf(stdout, "\n");
		i++;   /* NOTA: junto con el i++ del for, avanza de dos en dos */
	}
	return (0);
}
