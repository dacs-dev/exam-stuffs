/*
** searchable_array_bag.cpp
** ========================
** Implementación de searchable_array_bag.
**
** Esta clase no tiene datos propios: todo el estado (data, size) vive en
** array_bag. Por eso los constructores y el operador= simplemente delegan
** en la base array_bag — que ya sabe gestionar la memoria dinámica.
**
** El único método real que aporta esta clase es has(), que proporciona
** la búsqueda lineal prometida por searchable_bag.
*/

#include "searchable_array_bag.hpp"

// Constructor por defecto.
// array_bag() inicializa data=nullptr y size=0 automáticamente.
// searchable_bag y bag no tienen estado, así que no hay nada más que hacer.
searchable_array_bag::searchable_array_bag()
{
	// Cuerpo vacío: la inicialización la realiza array_bag en su ctor.
}

// Constructor de copia.
// Se pasa 'other' a array_bag(other) en la lista de inicialización para que
// array_bag haga la copia profunda (deep copy) del array dinámico.
// NOTA: No se llama a searchable_bag(other) porque searchable_bag no tiene estado.
// Con herencia virtual, el compilador llama al ctor por defecto de bag automáticamente
// (no necesitamos llamarlo explícitamente aquí).
searchable_array_bag::searchable_array_bag(const searchable_array_bag& other) : array_bag(other)
{
	// Cuerpo vacío: la copia profunda la gestiona array_bag.
}

// Operador de asignación.
// Patrón estándar: comprobación de auto-asignación + delegación en la base.
// array_bag::operator= se encarga de liberar la memoria antigua y copiar la nueva.
searchable_array_bag&	searchable_array_bag::operator=(const searchable_array_bag& other)
{
	if (this != &other)           // Evitar self-assignment (liberar y leer la misma memoria)
		array_bag::operator=(other); // Delegar la copia profunda en array_bag
	return (*this);               // Devolver *this permite encadenar asignaciones (a = b = c)
}

// Destructor.
// array_bag::~array_bag() libera el array dinámico (delete[] data).
// El destructor de searchable_array_bag solo necesita existir para que el
// polimorfismo funcione correctamente (cadena de destructores virtuales).
searchable_array_bag::~searchable_array_bag()
{
	// Cuerpo vacío: la memoria la libera ~array_bag().
}

// has() — implementación del método virtual puro de searchable_bag.
//
// Estrategia: BÚSQUEDA LINEAL.
// Un array_bag no mantiene ningún orden, por lo que la única opción es
// recorrer todos los elementos uno a uno.
//
// 'data' y 'size' son miembros protegidos heredados de array_bag,
// accesibles directamente desde esta clase hija.
//
// El calificador 'const' garantiza que has() no modifica el objeto,
// permitiendo llamarlo sobre instancias constantes (const searchable_array_bag&).
bool	searchable_array_bag::has(int value) const
{
	for (int i = 0; i < size; i++)
	{
		if (data[i] == value)
			return (true);   // Encontrado: salir inmediatamente (eficiencia)
	}
	return (false);          // No encontrado tras recorrer todo el array
}
