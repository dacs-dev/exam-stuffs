/*
** set.hpp
** =======
** Rol en la jerarquía:
**   'set' NO hereda de ninguna clase de la jerarquía bag. Es una clase
**   independiente que CONTIENE (composición) una referencia a searchable_bag.
**   Su responsabilidad es añadir semántica de CONJUNTO (sin duplicados) sobre
**   cualquier tipo de bolsa buscable.
**
** POR QUÉ EL MIEMBRO ES UNA REFERENCIA (searchable_bag&)
** -------------------------------------------------------
**   Una referencia en C++ tiene dos propiedades fundamentales:
**     1. Debe inicializarse en el momento de la construcción (no puede
**        quedar sin valor).
**     2. No puede reasignarse para apuntar a otro objeto después de creada.
**
**   Consecuencias para el OCF:
**     - Constructor por defecto IMPOSIBLE: no hay forma de inicializar _bag
**       sin recibir un objeto al que referenciarse.
**     - Operador de asignación IMPOSIBLE: no se puede cambiar a qué
**       searchable_bag apunta _bag una vez construido.
**     - Constructor de copia PROBLEMÁTICO: dos 'set' apuntarían al mismo
**       _bag, lo que crea aliasing peligroso.
**   → POR ESO los tres se declaran en PRIVADO y sin implementar: se
**     prohiben al resto del mundo.
**
** SEMÁNTICA DE CONJUNTO
** ---------------------
**   La propiedad clave de un conjunto matemático es que no contiene
**   elementos repetidos. Esto se implementa en insert(int): antes de
**   añadir, se pregunta has() y solo se inserta si el elemento es nuevo.
**   El insert(int*, int) del array NO hace esta comprobación (posible bug
**   de la solución original o diseño deliberado; se conserva sin modificar).
**
** CONST-CORRECTNESS
** -----------------
**   has() y print() son 'const' porque no modifican el conjunto.
**   get_bag() devuelve 'const searchable_bag&' para impedir que el llamador
**   modifique la bolsa interna a través de la referencia devuelta.
*/

#include "searchable_bag.hpp"

class set
{
	private:
		// Referencia a la bolsa buscable concreta (array o árbol).
		// Al ser referencia: vive tanto como el objeto externo que se le pase.
		// El 'set' NO es dueño del objeto — no lo crea ni lo destruye.
		searchable_bag&	_bag;

		// --- OCF deshabilitado (privado, sin implementar) ---
		// Constructor por defecto: imposible porque _bag debe inicializarse.
		set();
		// Constructor de copia: prohibido para evitar aliasing peligroso.
		set(const set& other);
		// Operador de asignación: imposible porque una referencia no se reasigna.
		set&	operator=(const set& other);

	public:
		// Único constructor válido: recibe la bolsa a la que referenciarse.
		// La lista de inicialización es OBLIGATORIA para inicializar _bag.
		set(searchable_bag& s_bag);

		// Destructor: no hace nada porque set no posee _bag.
		~set();

		// Devuelve la bolsa interna como referencia constante.
		// 'const searchable_bag&' impide modificarla desde fuera.
		// NOTA: el método en sí NO es const — podría serlo, pero la solución
		// original no lo marca así (se respeta sin cambiar).
		const searchable_bag&	get_bag();

		// Consulta si un valor existe en el conjunto.
		// 'const' porque no modifica el estado.
		bool	has(int value) const;

		// Inserta UN elemento: comprueba has() antes para garantizar unicidad.
		void insert (int);

		// Inserta un array de enteros. Llama directamente a _bag.insert()
		// sin comprobar duplicados (comportamiento de la solución original).
		void insert (int *, int);

		// Imprime el contenido del conjunto (delega en _bag.print()).
		void print() const;

		// Vacía el conjunto (delega en _bag.clear()).
		void clear();
};
