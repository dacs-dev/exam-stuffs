/*
** searchable_tree_bag.hpp
** =======================
** Rol en la jerarquía:
**   Versión "buscable" de tree_bag. Combina dos ramas igual que
**   searchable_array_bag, pero usando un árbol binario de búsqueda (BST)
**   en lugar de un array.
**
** PROBLEMA DEL DIAMANTE (misma solución que con array_bag)
** ---------------------------------------------------------
**              bag
**             /   \
**   searchable_bag  tree_bag   <- ambas usan "virtual public bag"
**             \   /
**        searchable_tree_bag
**
** Gracias a la herencia virtual en las bases, solo hay UNA instancia de bag.
**
** ESTRATEGIA DE BÚSQUEDA: BST recursivo
** ---------------------------------------
** A diferencia del array (desordenado, búsqueda O(n)), el árbol mantiene
** la propiedad BST: nodo->l < nodo->value <= nodo->r.
** Esto permite descartar mitades del árbol en cada nivel → O(log n) promedio.
**
** La función auxiliar privada 'search(node*, int)' encapsula la recursión
** para que has() pueda iniciarla desde la raíz con una llamada limpia.
*/

#pragma once

#include "tree_bag.hpp"
#include "searchable_bag.hpp"


class searchable_tree_bag : public searchable_bag, public tree_bag
{
	private:
		// Función auxiliar recursiva para la búsqueda BST.
		// Recibe el nodo actual y el valor buscado.
		// Es privada porque es un detalle de implementación; el usuario llama a has().
		// Es 'const' porque no modifica el árbol.
		bool	search(node* node, int value) const;

	public:
		// --- OCF ---
		// Constructor por defecto: tree_bag inicializa tree=nullptr.
		searchable_tree_bag();

		// Constructor de copia: delega en tree_bag(other) para hacer
		// la copia profunda (deep copy) del árbol mediante copy_node recursivo.
		searchable_tree_bag(const searchable_tree_bag& other);

		// Operador de asignación: delega en tree_bag::operator= para gestionar
		// la liberación del árbol antiguo y la copia profunda del nuevo.
		searchable_tree_bag&	operator=(const searchable_tree_bag& other);

		// Destructor: tree_bag::~tree_bag() destruye el árbol (destroy_tree recursivo).
		~searchable_tree_bag();

		// has() implementa el método virtual puro de searchable_bag.
		// Inicia la búsqueda BST recursiva desde la raíz (this->tree).
		bool	has(int value) const;
};
