/*
** ============================================================
**  bsq.h  —  Cabecera principal del ejercicio BSQ (Biggest Square)
** ============================================================
**
**  PROPÓSITO DE ESTE FICHERO
**  ─────────────────────────
**  Define el tipo de dato central (t_map) y declara las cuatro
**  funciones públicas que componen la solución.  El fichero bsq.c
**  las implementa; main.c las llama.
**
**  ESTRUCTURA t_map  (el "mapa" completo en memoria)
**  ──────────────────────────────────────────────────
**  rows     → número de filas del mapa (leído de la primera línea)
**  cols     → número de columnas (deducido de la primera fila de datos)
**  empty    → carácter que representa celda vacía  (p.ej. '.')
**  obstacle → carácter que representa obstáculo    (p.ej. 'o')
**  full     → carácter con el que se rellena el cuadrado mayor (p.ej. 'x')
**  grid     → array de cadenas; grid[i] es la fila i del mapa
**             (memoria dinámica, liberada por free_map)
**
**  FUNCIONES PÚBLICAS
**  ──────────────────
**  parse_map  → lee el fichero y rellena la estructura; devuelve 0 si error
**  solve      → aplica el algoritmo de prog. dinámica y marca el cuadrado mayor
**  print_map  → imprime el resultado en stdout
**  free_map   → libera toda la memoria dinámica del mapa
**
**  COMPILACIÓN
**  ───────────
**  cc -Wall -Wextra -Werror *.c
** ============================================================
*/

#pragma once

#include <stdio.h>   /* FILE, fprintf, getline, stdout, stderr */
#include <stdlib.h>  /* malloc, calloc, free */

/*
** t_map: contiene TODO lo que describe un mapa válido.
** Se pasa siempre por puntero para que las funciones puedan
** modificarla (rellenar campos, liberar memoria, etc.).
*/
typedef struct s_map
{
	int		rows;      /* Número de filas declaradas en la cabecera */
	int		cols;      /* Número de columnas (constante en todas las filas) */
	char	empty;     /* Carácter "vacío" — las celdas disponibles */
	char	obstacle;  /* Carácter "obstáculo" — celdas bloqueadas */
	char	full;      /* Carácter con el que se pinta el cuadrado mayor */
	char	**grid;    /* Array de punteros a cadena, uno por fila */
} t_map;

/* ── Prototipos ─────────────────────────────────────────── */

void	print_map(t_map *map);
void	free_map(t_map *map);
int		parse_map(t_map *map, FILE *file);
void	solve(t_map *map);
