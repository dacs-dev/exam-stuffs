/*
** =============================================================================
** CLASE vect2 — Vector 2D de enteros
** =============================================================================
** Esta clase modela un vector matemático de dos componentes enteras (_x, _y).
** Su objetivo pedagógico es demostrar la SOBRECARGA DE OPERADORES en C++98.
**
** CONCEPTOS QUE DEMUESTRA:
**   1. Forma Canónica Ortodoxa (Rule of Three en C++98)
**   2. Operadores de modificación (+=, -=, *=) vs. operadores de cálculo (+, -, *)
**   3. Pre-incremento vs. post-incremento: la firma con (int) dummy
**   4. operator[] doble: versión const (lectura) y no-const (escritura)
**   5. Funciones libres: operator<< y operator*(int, vect2) no pueden ser métodos
**   6. const-correctness: marcar con 'const' todo lo que no muta el objeto
** =============================================================================
*/

#pragma once

#include <iostream>
#include <string>

class vect2
{
	private:
		int _x;
		int _y;
	public:
		// -----------------------------------------------------------------
		// FORMA CANÓNICA ORTODOXA (las 4 grandes de C++98)
		// Todo objeto que gestiona recursos propios debe definirlas.
		// Aunque aquí no hay memoria dinámica, las declaramos explícitamente
		// para dejar claro el patrón y evitar comportamientos inesperados.
		// -----------------------------------------------------------------

		// 1. Constructor por defecto — inicializa el objeto a estado cero.
		vect2();

		// Constructor parametrizado — no es parte de la forma canónica pero
		// es la forma normal de construir un vector con valores concretos.
		vect2(int x, int y);

		// 2. Constructor de copia — crea una copia INDEPENDIENTE de 'other'.
		//    El compilador ya generaría uno aquí (sin punteros), pero lo
		//    declaramos explícitamente para demostrar el patrón.
		vect2(const vect2& other);

		// 3. Operador de asignación — copia 'other' en un objeto YA existente.
		//    Devuelve vect2& (referencia a *this) para permitir encadenamiento:
		//    a = b = c;  →  a.operator=(b.operator=(c))
		//    SIEMPRE incluye la autoasignación guard: if (this != &other)
		vect2&	operator=(const vect2& other);

		// 4. Destructor — libera recursos. Sin memoria dinámica queda vacío,
		//    pero debe estar presente para completar la forma canónica.
		~vect2();

		// -----------------------------------------------------------------
		// operator[] DOBLE — const-correctness en acción
		// -----------------------------------------------------------------
		// Versión NO-const: devuelve int& (referencia), lo que permite asignar:
		//   v[0] = 42;   — modifica _x directamente a través de la referencia.
		//   Solo puede llamarse sobre objetos no-const.
		int&	operator[](int index);

		// Versión CONST: devuelve int por valor (no referencia).
		//   Solo puede llamarse sobre objetos const (p. ej. en operator<<).
		//   Si devolviese int& desde un método const, el llamador podría
		//   modificar el estado interno del objeto — violación de const.
		int		operator[](int index) const;

		// -----------------------------------------------------------------
		// PRE vs. POST incremento/decremento
		// -----------------------------------------------------------------
		// PRE-incremento (++v): incrementa Y DEVUELVE el objeto ya modificado.
		//   Devuelve vect2& porque el objeto existente es el resultado útil.
		//   Más eficiente: no necesita crear ninguna copia temporal.
		vect2&	operator++();

		// POST-incremento (v++): guarda una copia, incrementa, devuelve la copia.
		//   El parámetro 'int' (sin nombre) es un "dummy" que el compilador usa
		//   SOLO para distinguir la firma de la del pre-incremento. No se usa.
		//   DEBE devolver vect2 por VALOR (no por referencia) porque la copia
		//   temporal es una variable local que desaparece al salir de la función.
		vect2	operator++(int);

		vect2&	operator--();
		vect2	operator--(int);

		// -----------------------------------------------------------------
		// OPERADORES DE MODIFICACIÓN vs. OPERADORES DE CÁLCULO
		// -----------------------------------------------------------------
		// Patrón general:
		//   op=  →  modifica *this, devuelve vect2& (referencia al mismo objeto).
		//   op   →  crea un temporal nuevo, no toca *this, devuelve vect2 por valor.
		//           Por eso es 'const': no modifica el objeto sobre el que se llama.
		//
		// Esto permite implementar op usando op= (DRY):
		//   vect2 operator+(const vect2& b) const  { vect2 tmp(*this); tmp += b; return tmp; }

		vect2&	operator*=(int toMult);
		// operator*(int) es método porque el operando izquierdo ES el objeto.
		// Devuelve por valor porque el resultado es un vector nuevo, no *this.
		// Es 'const' porque no modifica el objeto receptor.
		vect2	operator*(int toMult) const;

		vect2&	operator+=(const vect2& toAdd);
		vect2	operator+(const vect2& toAdd) const;

		vect2&	operator-=(const vect2& toSub);
		vect2	operator-(const vect2& toSub) const;

		// Operadores de comparación: solo leen, nunca mutan → obligatoriamente const.
		bool	operator==(const vect2& other) const;
		bool	operator!=(const vect2& other) const;

		// Negación unaria (-v): devuelve un nuevo vector con componentes negadas.
		// Es const porque no altera el objeto original.
		vect2	operator-() const;
};

// -----------------------------------------------------------------------------
// FUNCIONES LIBRES — por qué NO pueden ser métodos de la clase
// -----------------------------------------------------------------------------

// operator*(int, vect2): la sintaxis sería  3 * v
//   Si fuese método de vect2, el operando IZQUIERDO debería ser *this, es decir,
//   un vect2. Pero aquí el izquierdo es un int → imposible hacerlo método.
//   Se declara libre y delega en operator*= del objeto para reutilizar lógica.
vect2	operator*(int num, const vect2& toMult);

// operator<<: el operando izquierdo es std::ostream&, no un vect2.
//   Solo la clase std::ostream podría tenerlo como método, pero no podemos
//   modificarla. Por eso siempre es función libre.
//   Devuelve ostream& para permitir encadenamiento:  cout << v1 << v2;
//   Usa operator[] const del objeto para no romper la encapsulación.
std::ostream&	operator<<(std::ostream& os, const vect2& toPrint);
