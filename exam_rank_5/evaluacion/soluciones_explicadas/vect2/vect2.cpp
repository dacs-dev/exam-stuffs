/*
** =============================================================================
** vect2.cpp — Implementación comentada de la clase vect2
** =============================================================================
** Este fichero contiene la definición de todos los métodos y funciones libres
** declarados en vect2.hpp.
**
** LECTURA RECOMENDADA: empieza por los constructores (Forma Canónica),
** luego operator[], luego pre/post incremento, y finalmente los aritméticos.
** Las funciones libres al final explican el caso "el izquierdo no es el objeto".
** =============================================================================
*/

#include "vect2.hpp"

// =============================================================================
// FORMA CANÓNICA ORTODOXA — constructores y destructor
// =============================================================================

// Constructor por defecto: lista de inicialización en lugar de asignación
// en el cuerpo. Es más eficiente y el estilo correcto en C++.
// Garantiza que un vect2 sin argumentos queda en estado definido (0, 0).
vect2::vect2() : _x(0), _y(0)
{

}

// Constructor parametrizado: inicializa directamente con los valores dados.
// No es parte de la forma canónica pero es el modo habitual de crear vectores.
vect2::vect2(int x, int y) : _x(x), _y(y)
{

}

// Constructor de copia: toma el estado de 'other' para inicializar este objeto.
// Al no haber punteros ni recursos dinámicos, una copia miembro a miembro es
// suficiente. Con punteros habría que hacer una "deep copy".
vect2::vect2(const vect2& other) : _x(other._x), _y(other._y)
{

}

// Operador de asignación: a diferencia del constructor de copia, el objeto
// destino YA EXISTE y puede tener estado previo.
//
// GUARDIA DE AUTOASIGNACIÓN: "if (this != &other)"
//   Sin ella, "v = v;" copiaría los datos antes de limpiarlos → con recursos
//   dinámicos esto destruiría los datos fuente antes de copiarlos.
//   Aquí no hay punteros, pero el patrón es obligatorio en la forma canónica.
//
// Devuelve *this por referencia para habilitar encadenamiento: a = b = c;
vect2&	vect2::operator=(const vect2& other)
{
	if (this != &other)
	{
		_x = other._x;
		_y = other._y;
	}
	return (*this);
}

// Destructor: sin recursos dinámicos, el cuerpo queda vacío.
// Aun así se declara para completar la forma canónica y dejar claro que
// el programador pensó en él.
vect2::~vect2()
{

}

// =============================================================================
// operator[] — acceso indexado doble (const y no-const)
// =============================================================================

// Versión CONST: llamada cuando el objeto es const (ej: en operator<<).
//   Devuelve int por VALOR para no exponer una referencia a un miembro privado
//   desde un contexto const — eso permitiría modificarlo, rompiendo la promesa.
int		vect2::operator[](int index) const
{
	if (index == 0)
	{
		return (_x);
	}
	else
		return (_y);
}

// Versión NO-CONST: llamada cuando el objeto no es const.
//   Devuelve int& (referencia) para permitir asignación:
//     v[0] = 10;   ←  equivale a  v._x = 10;
//   El compilador elige automáticamente cuál versión invocar según el
//   contexto (const vs. no-const). Esto se llama "overload resolution".
int&	vect2::operator[](int index)
{
	if (index == 0)
	{
		return (_x);
	}
	else
		return (_y);
}

// =============================================================================
// Incremento y decremento — pre vs. post
// =============================================================================

// PRE-incremento (++v):
//   Modifica el objeto y devuelve una REFERENCIA al propio objeto ya modificado.
//   Ventaja: no crea copias temporales → más eficiente.
//   Retorna vect2& porque *this sigue existiendo al salir de la función.
vect2&	vect2::operator++()
{
	_x++;
	_y++;
	return (*this);
}

// POST-incremento (v++):
//   1. Guarda el estado ANTERIOR en 'tmp' (copia del objeto actual).
//   2. Modifica el objeto original.
//   3. Devuelve 'tmp' (el estado previo), que es lo que "ve" el llamador.
//
//   El parámetro 'int' (sin nombre) es solo un marcador sintáctico para
//   distinguir esta firma de la del pre-incremento. El compilador genera
//   automáticamente la llamada correcta: ++v llama a operator++() y
//   v++ llama a operator++(int) con un 0 implícito.
//
//   DEBE devolver por VALOR porque 'tmp' es una variable local: cuando la
//   función termina, su almacenamiento desaparece. Devolver una referencia
//   a 'tmp' sería un error (dangling reference).
vect2	vect2::operator++(int)
{
	vect2 tmp(*this);	// copia el estado actual (usa el constructor de copia)
	_x++;
	_y++;
	return (tmp);		// devuelve el estado previo por valor
}

// PRE-decremento: análogo al pre-incremento.
vect2&	vect2::operator--()
{
	_x--;
	_y--;
	return (*this);
}

// POST-decremento: análogo al post-incremento.
vect2	vect2::operator--(int)
{
	vect2 tmp(*this);
	_x--;
	_y--;
	return (tmp);
}

// =============================================================================
// Operadores aritméticos — patrón op= / op
// =============================================================================

// *=  — MODIFICA el objeto. Devuelve referencia a *this para encadenamiento.
vect2& vect2::operator*=(int to_mult)
{
	_x *= to_mult;
	_y *= to_mult;
	return (*this);
}

// *   — NO modifica el objeto (es const). Crea un temporal, lo multiplica y
//   lo devuelve por VALOR. No puede devolver *this porque eso modificaría el
//   objeto original, rompiendo la semántica de *.
//   Alternativa idiomática: tmp *= to_mult; return tmp;  (delega en operator*=)
vect2 vect2::operator*(int to_mult) const
{
	vect2 tmp(*this);
	tmp._x = _x * to_mult;
	tmp._y = _y * to_mult;
	return (tmp);
}

// +=  — Suma otro vector al actual. Modifica *this, devuelve referencia.
vect2& vect2::operator+=(const vect2& to_add)
{
	_x += to_add._x;
	_y += to_add._y;
	return (*this);
}

// +   — Devuelve un NUEVO vector con la suma. No toca *this.
//   El patrón idiomático es: crear copia de *this, aplicarle +=, devolver copia.
//   Así operator+ reutiliza la lógica de operator+= sin duplicar código.
vect2 vect2::operator+(const vect2& to_add) const
{
	vect2 tmp(*this);
	tmp._x = _x + to_add._x;
	tmp._y = _y + to_add._y;
	return (tmp);
}

// -=  — Análogo a +=.
vect2& vect2::operator-=(const vect2& to_sub)
{
	_x -= to_sub._x;
	_y -= to_sub._y;
	return (*this);
}

// -   — Análogo a +.
vect2 vect2::operator-(const vect2& to_sub) const
{
	vect2 tmp(*this);
	tmp._x = _x - to_sub._x;
	tmp._y = _y - to_sub._y;
	return (tmp);
}

// Negación unaria (-v): devuelve un vector con ambas componentes negadas.
// No toca el original → es const y devuelve por valor.
vect2 vect2::operator-() const
{
	vect2 tmp(*this);
	tmp._x = -_x;
	tmp._y = -_y;
	return (tmp);
}

// =============================================================================
// Comparación
// =============================================================================

// != Devuelve true si alguna componente difiere. No muta → const.
bool vect2::operator!=(const vect2& other) const
{
	if (_x == other._x && _y == other._y)
		return (false);
	else
		return (true);
}

// == Devuelve true si ambas componentes son iguales. No muta → const.
bool vect2::operator==(const vect2& other) const
{
	if (_x == other._x && _y == other._y)
		return (true);
	else
		return (false);
}

// =============================================================================
// FUNCIONES LIBRES — el operando izquierdo no es un vect2
// =============================================================================

// operator*(int, vect2) — para la sintaxis: 3 * v
//   Si fuera método de vect2, *this sería el operando izquierdo (el int),
//   lo cual es imposible porque int no es nuestra clase.
//   Solución: función libre que reutiliza operator*= del objeto.
//   El parámetro vect2 se pasa por const& para evitar una copia innecesaria.
vect2 operator*(int num, const vect2& to_mult)
{
	vect2 temp(to_mult);	// copia para no modificar el original
	temp *= num;			// delega en el método operator*= ya probado
	return temp;
}

// operator<<(ostream, vect2) — para: cout << v;
//   El operando izquierdo es std::ostream&, que pertenece a la biblioteca
//   estándar: no podemos añadirle métodos. Por eso siempre es función libre.
//
//   Recibe ostream por referencia (no por valor) porque ostream no es copiable.
//   Devuelve ostream& para encadenamiento: cout << v1 << v2 << endl;
//
//   Usa toPrint[0] y toPrint[1] en lugar de acceder a _x/_y directamente
//   porque operator[] const respeta la encapsulación y está disponible
//   para objetos const (el parámetro es const vect2&).
std::ostream& operator<<(std::ostream& os, const vect2& toPrint)
{
	os << "{" << toPrint[0] << ", " << toPrint[1] << "}";
	return os;
}
