/*
** searchable_array_bag.hpp
** ========================
** Rol en la jerarquía:
**   Esta clase CONCRETA combina dos ramas de herencia:
**     - searchable_bag : añade la capacidad de buscar (método has)
**     - array_bag      : proporciona el almacenamiento con array dinámico
**
** PROBLEMA DEL DIAMANTE y HERENCIA VIRTUAL
** -----------------------------------------
** La jerarquía completa tiene forma de diamante:
**
**              bag          <- clase raíz abstracta
**             /   \
**   searchable_bag  array_bag   <- ambas heredan de bag con "virtual public bag"
**             \   /
**        searchable_array_bag   <- hereda de las dos anteriores
**
** Sin herencia virtual, searchable_array_bag contendría DOS copias de bag
** (una por cada camino del diamante), lo que generaría ambigüedad al llamar
** a insert(), print(), etc.
**
** Gracias a que searchable_bag y array_bag declaran su herencia como
** "virtual public bag", el compilador garantiza que solo existe UNA instancia
** compartida de bag dentro de searchable_array_bag.
**
** El estudiante NO tiene que volver a poner "virtual" aquí; solo hereda de
** las dos clases intermedias de forma normal (public).
*/

#pragma once

#include "array_bag.hpp"
#include "searchable_bag.hpp"

// searchable_array_bag hereda de ambas ramas.
// Gracias a la herencia virtual declarada en las bases, no hay duplicado de bag.
class searchable_array_bag : public searchable_bag, public array_bag
{
	public:
		// --- OCF (Orthodox Canonical Form) ---
		// Constructor por defecto: inicializa a través de array_bag (y por ende bag).
		searchable_array_bag();

		// Constructor de copia: delega en array_bag(other) para copiar data y size.
		searchable_array_bag(const searchable_array_bag& other);

		// Operador de asignación: delega en array_bag::operator= para gestionar
		// la memoria dinámica correctamente (liberar vieja, copiar nueva).
		searchable_array_bag&	operator=(const searchable_array_bag& other);

		// Destructor: array_bag ya gestiona la liberación de `data`, no hace falta nada extra.
		~searchable_array_bag();

		// has() implementa el método virtual puro de searchable_bag.
		// Búsqueda LINEAL: recorre el array de 0 a size-1.
		// Complejidad O(n) — apropiada para estructura no ordenada.
		bool	has(int value) const;
};
