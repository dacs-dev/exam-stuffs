/*
** =============================================================================
** BIGINT — Enteros de Precisión Arbitraria en Base 10
** =============================================================================
**
** CONCEPTO FUNDAMENTAL: ¿Qué es un "bignum"?
** --------------------------------------------
** Los tipos nativos de C++ (int, long, unsigned long long) tienen un tamaño
** fijo en memoria (32 o 64 bits). Cuando necesitamos representar números
** arbitrariamente grandes (p.ej. el factorial de 100), necesitamos almacenar
** los dígitos explícitamente. Esto se llama "precisión arbitraria".
**
** REPRESENTACIÓN INTERNA: std::string
** -------------------------------------
** En esta implementación los dígitos se guardan como caracteres ASCII en un
** std::string. El dígito más significativo está en _str[0] y el menos
** significativo en _str[_str.size()-1]. Ejemplo: el número 1234 se almacena
** como la cadena "1234".
**
** Ventajas de usar std::string:
**   - Tamaño dinámico sin gestión manual de memoria.
**   - La comparación lexicográfica (std::string::operator<) coincide con la
**     comparación numérica SI ambas cadenas tienen la misma longitud
**     (misma cantidad de dígitos). Por eso la comparación primero compara
**     longitudes y luego compara lexicográficamente.
**
** FORMA CANÓNICA (NORMALIZACIÓN)
** --------------------------------
** Para que la representación sea única (sin ambigüedad), siempre eliminamos
** los ceros a la izquierda: "007" y "7" representan el mismo número, pero
** rompería la comparación por longitud. La función validate() garantiza que
** nunca haya ceros a la izquierda.
**
** OPERADORES DE DESPLAZAMIENTO EN BASE 10 (NO son bit-shifts)
** --------------------------------------------------------------
** operator<< y operator<<= : "digit-shift izquierda" → multiplica por 10^n
**   Equivale a añadir n ceros al final de la cadena. Ejemplo:
**   bigint(5) << 3  →  "5000"  (5 * 10^3 = 5000)
**
** operator>>= : "digit-shift derecha" → divide por 10^n (división entera)
**   Equivale a eliminar los últimos n dígitos. Ejemplo:
**   bigint(1234) >>= 2  →  "12"  (1234 / 10^2 = 12)
**
**   ATENCIÓN: estos operadores usan la sintaxis << y >> pero NO son
**   desplazamientos de bits. Trabajan en base 10 sobre la cadena de dígitos.
**
** FORMA CANÓNICA ORTODOXA (The Orthodox Canonical Form)
** -------------------------------------------------------
** En C++98, una clase bien formada debe implementar los "Big Four":
**   1. Constructor por defecto
**   2. Constructor de copia
**   3. Operador de asignación
**   4. Destructor
** Esto garantiza que las copias y asignaciones funcionen correctamente.
**
** CONST-CORRECTNESS
** ------------------
** Los métodos que NO modifican el objeto se marcan const (p.ej. get_str(),
** operator==, operator<). Esto permite llamarlos sobre objetos const como
**   const bigint a(42);  →  a puede usarse en operator+, comparaciones, etc.
**
** operator<< COMO FUNCIÓN LIBRE
** ------------------------------
** El operator<< para std::ostream NO puede ser método miembro de bigint porque
** el operando izquierdo (os) no es bigint. Se declara como función libre fuera
** de la clase pero necesita acceso a get_str() (público).
** =============================================================================
*/

#pragma once

#include <string>
#include <sstream>
#include <iostream>

// Clase que representa un entero no negativo de precisión arbitraria.
// Los dígitos se almacenan en forma de cadena ASCII, dígito más significativo
// primero (_str[0]), sin ceros a la izquierda (excepto el propio "0").
class bigint
{
	private:
		std::string	_str;  // Almacén interno: cadena de dígitos decimales

	public:
		// ---------------------------------------------------------------
		// FORMA CANÓNICA ORTODOXA
		// ---------------------------------------------------------------

		// Constructor por defecto: inicializa a 0
		bigint();

		// Constructor desde unsigned int: convierte el número a cadena
		bigint(unsigned int n);

		// Constructor de copia: copia la cadena de dígitos
		bigint(const bigint& other);

		// Operador de asignación: protege contra autoasignación (this != &other)
		bigint&	operator=(const bigint& other);

		// Destructor: no hay gestión dinámica propia (string se destruye solo)
		~bigint();

		// ---------------------------------------------------------------
		// ACCESO E INVARIANTE
		// ---------------------------------------------------------------

		// Devuelve la representación en cadena del número. Es const porque
		// no modifica el objeto. Necesario para operator<< (función libre).
		std::string	get_str() const;

		// Elimina ceros a la izquierda para mantener la forma canónica.
		// Llamada tras operaciones que puedan introducir ceros iniciales.
		void	validate();

		// ---------------------------------------------------------------
		// INCREMENTO
		// ---------------------------------------------------------------

		// Pre-incremento: ++b → incrementa y devuelve referencia al propio objeto
		bigint&	operator++();

		// Post-incremento: b++ → guarda copia, incrementa, devuelve copia previa
		bigint	operator++(int);

		// ---------------------------------------------------------------
		// SUMA
		// ---------------------------------------------------------------

		// Suma en el lugar (modifica *this). Devuelve referencia para encadenar:
		//   c += a += b;
		bigint&	operator+=(const bigint& other);

		// Suma sin modificar: crea copia, le aplica +=, la devuelve por valor
		bigint	operator+(const bigint& other) const;

		// ---------------------------------------------------------------
		// DESPLAZAMIENTO DECIMAL (BASE 10, NO bits)
		// ---------------------------------------------------------------

		// >>= obj: divide entre 10^obj eliminando los últimos obj dígitos.
		// Acepta un bigint para poder desplazar cantidades > UINT_MAX (aunque
		// en la práctica el desplazamiento se limita al tamaño de la cadena).
		bigint&	operator>>=(const bigint& obj);

		// <<= n: multiplica por 10^n añadiendo n ceros al final de _str.
		// Devuelve referencia para permitir encadenamiento: (d <<= 4) >>= 2
		bigint&	operator<<=(unsigned int n);

		// << n: versión sin modificación. Crea copia temporal y aplica <<=.
		bigint	operator<<(unsigned int n) const;

		// ---------------------------------------------------------------
		// COMPARACIÓN
		// ---------------------------------------------------------------
		// Estrategia:
		//   1. Si las longitudes difieren, el número más largo es mayor
		//      (gracias a la forma canónica: sin ceros a la izquierda).
		//   2. Si las longitudes coinciden, la comparación lexicográfica de
		//      std::string equivale a la comparación numérica (mismo número
		//      de dígitos → el orden ASCII de '0'..'9' es el orden numérico).
		// Todos los demás operadores se implementan en términos de operator<
		// para no duplicar lógica.

		bool	operator==(const bigint& other) const;
		bool	operator!=(const bigint& other) const;
		bool	operator<(const bigint& other) const;
		bool	operator>(const bigint& other) const;
		bool	operator<=(const bigint& other) const;
		bool	operator>=(const bigint& other) const;
};

// Función libre para imprimir un bigint en cualquier std::ostream.
// No puede ser miembro porque el operando izquierdo es os (ostream), no bigint.
// Devuelve os por referencia para permitir el encadenamiento:
//   std::cout << a << " y " << b << std::endl;
std::ostream&	operator<<(std::ostream& os, const bigint& toPrint);
