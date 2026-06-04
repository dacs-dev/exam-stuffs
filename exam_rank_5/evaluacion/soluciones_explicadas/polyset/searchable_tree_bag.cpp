/*
** searchable_tree_bag.cpp
** =======================
** Implementación de searchable_tree_bag.
**
** La pieza clave de este fichero es la función auxiliar 'search', que
** implementa la búsqueda en un Árbol Binario de Búsqueda (BST) de forma
** recursiva aprovechando la propiedad de orden del árbol.
**
** Los constructores y operador= son idénticos en estructura a los de
** searchable_array_bag, pero delegando en tree_bag en lugar de array_bag.
*/

#include "searchable_tree_bag.hpp"

// search() — función auxiliar privada para búsqueda BST recursiva.
//
// PROPIEDAD BST que aprovechamos:
//   - Todos los valores en el subárbol izquierdo son MENORES que nodo->value.
//   - Todos los valores en el subárbol derecho son MAYORES o IGUALES que nodo->value.
//
// CASOS BASE:
//   1. node == nullptr → hemos llegado a una hoja inexistente: el valor NO está.
//   2. node->value == value → encontrado.
//
// CASO RECURSIVO:
//   - Si value < node->value → solo puede estar en el subárbol IZQUIERDO.
//   - Si value > node->value → solo puede estar en el subárbol DERECHO.
//   Así descartamos la mitad del árbol en cada llamada: O(log n) promedio.
//
// El parámetro se llama también 'node' (mismo nombre que el struct), lo
// cual es válido en C++ porque el parámetro oculta el nombre del tipo
// dentro del ámbito de la función.
bool	searchable_tree_bag::search(node* node, int value) const
{
	if (!node)                          // Caso base 1: nodo nulo → no encontrado
		return (false);
	if (node->value == value)           // Caso base 2: encontrado exactamente
		return (true);
	else if (value < node->value)       // El valor es menor: ir a la rama izquierda
		return (search(node->l, value));
	else                                // El valor es mayor: ir a la rama derecha
		return (search(node->r, value));
}

// Constructor por defecto.
// tree_bag() inicializa tree=nullptr; no hay nada más que inicializar.
searchable_tree_bag::searchable_tree_bag()
{
	// Cuerpo vacío.
}

// Constructor de copia.
// Delega en tree_bag(other): este llama a copy_node() recursivamente para
// duplicar toda la estructura del árbol (deep copy).
// searchable_bag no tiene estado → no necesita inicialización explícita.
searchable_tree_bag::searchable_tree_bag(const searchable_tree_bag& other) : tree_bag(other)
{
	// Cuerpo vacío: la copia profunda la gestiona tree_bag.
}

// Operador de asignación.
// Mismo patrón que en searchable_array_bag: protección ante self-assignment
// y delegación en la base que sabe gestionar la memoria del árbol.
searchable_tree_bag&	searchable_tree_bag::operator=(const searchable_tree_bag& other)
{
	if (this != &other)            // Protección anti-autoasignación
		tree_bag::operator=(other); // Destruye árbol antiguo, copia el nuevo en profundidad
	return (*this);
}

// Destructor.
// tree_bag::~tree_bag() llama a destroy_tree() recursivo para liberar
// cada nodo del árbol. Este destructor solo encadena la cadena de llamadas.
searchable_tree_bag::~searchable_tree_bag()
{
	// Cuerpo vacío: la limpieza la hace ~tree_bag().
}

// has() — implementación del método virtual puro de searchable_bag.
//
// Simplemente inicia la búsqueda recursiva BST desde la raíz del árbol.
// 'this->tree' es el puntero al nodo raíz heredado de tree_bag (miembro protegido).
// El qualificador 'const' garantiza que no se modifica el árbol.
bool	searchable_tree_bag::has(int value) const
{
	return (search(this->tree, value)); // Delegar toda la lógica en search()
}
