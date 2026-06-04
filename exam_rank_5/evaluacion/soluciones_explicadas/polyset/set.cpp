/*
** set.cpp
** =======
** Implementación de la clase set.
**
** set actúa como un ADAPTADOR (patrón Adapter/Wrapper): envuelve cualquier
** searchable_bag y añade la semántica de conjunto (sin duplicados) encima.
**
** Todos los métodos que no añaden lógica propia simplemente DELEGAN en _bag,
** aprovechando el polimorfismo: _bag puede ser una searchable_array_bag o una
** searchable_tree_bag, y el comportamiento correcto se selecciona en tiempo
** de ejecución (dispatch virtual).
*/

#include "set.hpp"
#include "searchable_array_bag.hpp"  // Incluido aunque set.hpp ya include searchable_bag.hpp
                                      // (la solución original lo incluye; se respeta)

// Constructor.
// La lista de inicialización ': _bag(s_bag)' es OBLIGATORIA:
//   - _bag es una referencia → solo puede inicializarse en la lista de inicialización,
//     no dentro del cuerpo del constructor.
//   - El objeto al que apunta (s_bag) debe seguir vivo durante toda la vida del set.
//     El llamador es responsable de garantizar ese ciclo de vida.
set::set(searchable_bag& s_bag) : _bag(s_bag)
{
	// Cuerpo vacío: _bag ya quedó inicializada en la lista.
}

// Destructor.
// set NO es dueño de _bag (no lo creó con new), así que NO lo destruye.
// Si hiciéramos 'delete &_bag' aquí sería un error grave (double-free o
// liberar memoria de pila).
set::~set()
{
	// Cuerpo vacío: no gestionamos la vida de _bag.
}

// get_bag() — acceso de solo lectura a la bolsa interna.
// Devuelve 'const searchable_bag&' para que el llamador no pueda llamar
// a métodos no-const de _bag a través de esta referencia.
// Ejemplo de uso en main: sa.get_bag().print()
const searchable_bag&	set::get_bag()
{
	return (_bag);
}

// has() — comprueba si un valor existe en el conjunto.
// Delega en _bag.has(), que tendrá el comportamiento polimórfico correcto:
//   - búsqueda lineal si _bag es searchable_array_bag
//   - búsqueda BST si _bag es searchable_tree_bag
bool	set::has(int value) const
{
	return (_bag.has(value));
}

// insert(int) — inserción con garantía de unicidad (semántica de conjunto).
//
// CLAVE: antes de insertar se llama a has().
// Si el elemento ya está → no se inserta → no hay duplicados.
// Si no está → se inserta en la bolsa subyacente.
//
// Esta lógica es LO QUE DIFERENCIA set de una bolsa ordinaria.
void set::insert (int value)
{
	if (!(has(value)))      // Solo insertar si el valor NO está ya en el conjunto
		_bag.insert(value);  // Delegar la inserción real en la bolsa subyacente
}

// insert(int*, int) — inserción de un array completo.
// En esta versión NO se comprueba unicidad antes de insertar:
// se delega directamente en _bag.insert(data, size).
// La implementación original de tree_bag/array_bag puede gestionar duplicados
// o no según su lógica interna.
void set::insert (int *data, int size)
{
	for (int i = 0; i < size; i++)
		_bag.insert(data[i]);  // Sin comprobación de has() — comportamiento original
}

// print() — imprime el contenido del conjunto.
// Delega en _bag.print(): el formato de salida depende de la implementación
// concreta (array imprime en orden de inserción, tree en in-order).
void set::print() const
{
	_bag.print();
}

// clear() — vacía el conjunto.
// Delega en _bag.clear(): libera/reinicia la estructura de datos interna
// (el array dinámico o todos los nodos del árbol).
void set::clear()
{
	_bag.clear();
}
