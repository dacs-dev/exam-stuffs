/*
** =============================================================================
** BIGINT — Implementación comentada
** =============================================================================
**
** Este fichero implementa todos los métodos de la clase bigint.
** Los números se representan como cadenas de dígitos decimales (base 10),
** dígito más significativo primero, sin ceros a la izquierda.
**
** RESUMEN DE ALGORITMOS CLAVE:
**
** 1. SUMA CON ACARREO (operator+=)
**    Imita la suma manual "de derecha a izquierda":
**      - Se recorren ambas cadenas desde el último dígito hacia el primero.
**      - Se suma d1 + d2 + carry. El dígito resultado es sum % 10.
**      - El acarreo para la siguiente posición es sum / 10.
**      - Los dígitos del resultado se acumulan al revés; al final se invierte.
**
** 2. DESPLAZAMIENTO DECIMAL (<<= y >>=)
**    NO son desplazamientos de bits. Operan sobre la cadena de dígitos:
**      - <<= n : append(n, '0')  →  multiplica por 10^n
**      - >>= n : erase(size-n, n) →  divide entera por 10^n
**
** 3. COMPARACIÓN (operator<)
**    Primero longitud (más dígitos → mayor número, gracias a la forma canónica),
**    luego comparación lexicográfica si las longitudes son iguales.
** =============================================================================
*/

#include "bigint.hpp"

// =============================================================================
// CONSTRUCTORES Y DESTRUCTOR
// =============================================================================

// Constructor por defecto: el cero se representa como la cadena "0".
// La lista de inicialización `: _str("0")` es más eficiente que asignar
// dentro del cuerpo porque inicializa directamente.
bigint::bigint() : _str("0")
{

}

// Constructor desde unsigned int.
// std::ostringstream actúa como un "printf a string": convierte el entero
// a su representación decimal automáticamente.
// Se llama validate() al final por robustez (aunque unsigned int no puede
// producir ceros iniciales, es buena práctica mantener el invariante).
bigint::bigint(unsigned int n)
{
	if (n == 0)
	{
		// Caso especial: evitamos que ostringstream produzca cadena vacía
		_str = "0";
		return ;
	}
	std::ostringstream	ss;
	ss << n;           // Convierte el número a string decimal
	_str = ss.str();
	validate();        // Garantiza forma canónica (sin ceros a la izquierda)
}

// Constructor de copia: inicializa _str con la cadena del otro bigint.
// La lista de inicialización es preferible al cuerpo vacío con asignación.
bigint::bigint(const bigint& other) :_str(other.get_str())
{

}

// Operador de asignación de copia.
// IMPORTANTE: verificar autoasignación (a = a) antes de copiar, porque si
// no, estaríamos destruyendo el origen antes de copiar.
bigint& bigint::operator=(const bigint& other)
{
	if (this != &other)       // Guarda contra a = a
		_str = other.get_str();
	return (*this);           // Devuelve *this para permitir a = b = c
}

// Destructor vacío: std::string gestiona su propia memoria,
// no necesitamos liberar nada manualmente.
bigint::~bigint()
{

}

// =============================================================================
// ACCESO E INVARIANTE
// =============================================================================

// Getter const: devuelve la cadena de dígitos por valor.
// La palabra const al final significa que este método no modifica el objeto,
// por lo que puede llamarse sobre instancias const: const bigint a(5); a.get_str();
std::string bigint::get_str() const
{
	return (_str);
}

// Elimina los ceros a la izquierda para mantener la FORMA CANÓNICA.
// Ejemplo: si _str fuera "007" tras alguna operación, lo convierte a "7".
// Sin esta normalización, la comparación por longitud de cadena fallaría:
//   "007".size() == 3  pero  "7".size() == 1 → comparación incorrecta.
// El caso "0" es especial: si toda la cadena son ceros, se deja como "0".
void bigint::validate()
{
	if (_str.empty() || _str == "0")
		return ;           // Nada que hacer para el cero o cadena vacía
	size_t i = 0;
	// Avanza mientras haya ceros iniciales
	while (i < _str.size() && _str[i] == '0')
		i++;
	if (i > 0)
		_str.erase(0, i);  // Elimina los primeros i caracteres ('0' iniciales)
}

// =============================================================================
// INCREMENTO
// =============================================================================

// Pre-incremento: ++b
// Reutiliza operator+= para no duplicar la lógica de suma.
// Devuelve referencia al propio objeto ya incrementado.
bigint& bigint::operator++()
{
	*this += bigint(1);
	return (*this);
}

// Post-incremento: b++
// El parámetro (int) ficticio es la convención de C++ para distinguir
// pre de post-incremento. No se usa pero debe estar en la firma.
// Guarda una copia ANTES de incrementar y devuelve esa copia.
bigint bigint::operator++(int)
{
	bigint	tmp(*this);    // Copia del estado antes de incrementar
	*this += bigint(1);
	return (tmp);          // Devuelve el valor anterior (por valor, no referencia)
}

// =============================================================================
// SUMA CON ACARREO
// =============================================================================

// Función auxiliar estática (solo visible en este fichero).
// Convierte una std::string de dígitos decimales a unsigned long long.
// Se necesita para operator>>= que recibe un bigint y necesita saber
// cuántos dígitos eliminar.
// LIMITACIÓN: si el bigint es mayor que ULLONG_MAX, habrá overflow.
// En la práctica, el desplazamiento nunca supera el tamaño de la cadena.
static unsigned long long str_to_number(const std::string& str)
{
	unsigned long long result = 0;
	if (str.empty() || str == "0")
		return (0);
	// Procesa cada dígito: multiplica el acumulado por 10 y suma el nuevo dígito.
	// result * 10 desplaza el número una posición decimal hacia la izquierda.
	for (size_t i = 0; i < str.size(); i++)
		result = result * 10 + (str[i] - '0');
	return (result);
}

// SUMA CON ACARREO — el algoritmo central de bignum.
// Itera de derecha a izquierda (dígito menos significativo primero),
// igual que hacemos en papel. Motivo: el acarreo se propaga hacia la izquierda.
bigint& bigint::operator+=(const bigint& other)
{
	int i = _str.size() - 1;         // Índice del último dígito de *this
	int j = other._str.size() - 1;   // Índice del último dígito de other
	int carry = 0;                    // Acarreo inicial: 0
	std::string	result;              // Acumulará los dígitos del resultado (al revés)

	// Continúa mientras queden dígitos en alguno de los dos números O haya acarreo.
	// La condición `|| carry` garantiza que el último acarreo genere un dígito extra.
	// Ejemplo: 99 + 1 → carry=1 sobrevive hasta generar el dígito "1" de "100".
	while (i >= 0 || j >= 0 || carry)
	{
		// Si el índice es negativo, ya no quedan dígitos en ese número → usar 0
		int d1 = (i >= 0) ? _str[i] - '0' : 0;
		int d2 = (j >= 0) ? other._str[j] -'0' : 0;

		int sum = d1 + d2 + carry;     // Suma de los dos dígitos más el acarreo
		result += (sum % 10) + '0';    // Dígito resultado: el módulo convertido a char
		carry = sum / 10;              // Nuevo acarreo: 0 si sum < 10, 1 si sum >= 10
		i--;
		j--;
	}
	// Los dígitos se añadieron de menos a más significativo, hay que invertir.
	// Ejemplo: sumar 9+9=18 → result="8" luego "81", invertido → "18"
	std::reverse(result.begin(), result.end());
	_str = result;
	return (*this);
}

// Suma sin modificar el objeto (const): crea una copia temporal,
// le aplica +=, y la devuelve por valor.
// Así reutiliza la lógica de += sin duplicar código.
bigint bigint::operator+(const bigint& other) const
{
	bigint	tmp(*this);
	tmp += other;
	return (tmp);
}

// =============================================================================
// DESPLAZAMIENTO DECIMAL (BASE 10) — NO son bit-shifts
// =============================================================================

// >>= obj: "digit-shift derecha" → divide entera por 10^obj.
// Equivale a quitar los últimos `shift` dígitos de la cadena.
// Ejemplo: "13370000" >>= 4  →  erase últimos 4 chars  →  "1337"
//          "42" >>= 5        →  shift >= size → resultado "0"
bigint& bigint::operator>>=(const bigint& obj)
{
	// Convierte el bigint de desplazamiento a entero nativo para poder
	// compararlo con el tamaño de la cadena y usarlo en erase().
	unsigned long long shift = str_to_number(obj._str);

	if (shift >= _str.size())
		// Si eliminamos todos los dígitos o más, el resultado es cero
		_str = "0";
	else
		// Elimina los últimos `shift` caracteres (dígitos menos significativos)
		_str.erase(_str.size() - shift, shift);
	return (*this);
}

// <<= n: "digit-shift izquierda" → multiplica por 10^n.
// Equivale a añadir n ceros al final de la cadena.
// Ejemplo: "1337" <<= 4  →  append 4 ceros  →  "13370000"
// EXCEPCIÓN: si el número es "0", no se añaden ceros (0 * 10^n = 0, no "000").
// Devuelve referencia para permitir encadenar: (d <<= 4) >>= 2
bigint& bigint::operator<<=(unsigned int shift)
{
	if (_str != "0")
		_str.append(shift, '0');  // Añade `shift` copias del carácter '0'
	return (*this);
}

// << n: versión const de <<=. Crea copia temporal, la modifica y la devuelve.
// Permite usar el operador en expresiones sin modificar el objeto original:
//   bigint resultado = b << 10;  // b no cambia
bigint bigint::operator<<(unsigned int shift) const
{
	bigint	tmp(*this);
	if (tmp._str != "0")
		tmp._str.append(shift, '0');
	return (tmp);
}

// =============================================================================
// OPERADORES DE COMPARACIÓN
// =============================================================================

// Igualdad: dos bigints son iguales si sus cadenas de dígitos son idénticas.
// Esto es correcto gracias a la forma canónica (sin ceros a la izquierda):
// el mismo número siempre tiene la misma representación.
bool bigint::operator==(const bigint& other) const
{
	return (_str == other._str);
}

// Desigualdad: negación de la igualdad. No duplica la lógica.
bool bigint::operator!=(const bigint& other) const
{
	return (!(_str == other._str));
}

// Menor que: el algoritmo de dos pasos aprovecha la forma canónica.
//
// PASO 1 — Comparar longitudes:
//   Si ambas cadenas no tienen la misma longitud, el número con MENOS dígitos
//   es el menor. Ejemplo: "99" (2 dígitos) < "100" (3 dígitos).
//   Esto SOLO funciona si no hay ceros a la izquierda (forma canónica),
//   porque "099" y "99" tienen diferente longitud pero representan el mismo valor.
//
// PASO 2 — Comparar lexicográficamente (misma longitud):
//   Cuando ambas cadenas tienen igual número de dígitos, el orden ASCII de
//   '0' < '1' < ... < '9' coincide con el orden numérico. std::string::operator<
//   compara carácter a carácter de izquierda a derecha, que es el orden correcto
//   para números con igual número de dígitos. Ejemplo: "42" < "99" → '4' < '9'.
bool bigint::operator<(const bigint& other) const
{
	if (_str.size() != other._str.size())
		return (_str.size() < other._str.size());  // Paso 1: longitud
	return (_str < other._str);                    // Paso 2: lexicográfico
}

// Mayor que: se implementa invirtiendo los operandos en operator<.
// "a > b" equivale a "b < a". Reutiliza la lógica sin duplicar.
bool bigint::operator>(const bigint& other) const
{
	return (other < *this);
}

// Menor o igual: es verdad cuando NO es mayor estricto.
bool bigint::operator<=(const bigint& other) const
{
	return (!(*this > other));
}

// Mayor o igual: es verdad cuando NO es menor estricto.
bool bigint::operator>=(const bigint& other) const
{
	return (!(*this < other));
}

// =============================================================================
// OPERADOR DE IMPRESIÓN — función libre (no miembro)
// =============================================================================

// operator<< se implementa como función LIBRE porque el operando izquierdo
// es un std::ostream (no un bigint). Si fuera miembro de bigint, tendría que
// estar al revés: bigint.operator<<(ostream), lo cual no tiene sentido.
//
// Accede a los datos a través del getter público get_str() (no necesita
// ser friend porque get_str() es public).
//
// Devuelve os por referencia para permitir encadenar operaciones:
//   std::cout << a << " + " << b << " = " << (a + b) << std::endl;
// Cada << devuelve el mismo os, que se pasa a la siguiente llamada.
std::ostream& operator<<(std::ostream& os, const bigint& toPrint)
{
	os << toPrint.get_str();
	return (os);
}
