# Evaluación — Exam Rank 05 (módulo CPP) · simulacro

Carpeta lista para practicar **como en el examen**: sin autocompletado, sin
linter, solo el enunciado, los ficheros que da el examen y un tester.

## Estructura

```
evaluacion/
├── grademe.sh                 ← tester maestro (ejecútalo desde aquí)
├── _lib.sh                    ← helpers de los testers (no tocar)
├── .solutions/                ← soluciones de referencia OCULTAS (no mirar salvo repaso)
├── level_1/                   ← C++  (-Wall -Wextra -Werror -std=c++98)
│   ├── vect2/    (escribes: vect2.hpp  vect2.cpp)
│   ├── bigint/   (escribes: bigint.hpp bigint.cpp)
│   └── polyset/  (escribes: searchable_array_bag.{hpp,cpp}
│                            searchable_tree_bag.{hpp,cpp} set.{hpp,cpp})
└── level_2/                   ← C  (-Wall -Wextra -Werror)
    ├── bsq/            (escribes: *.c *.h ; mapas de prueba en maps/)
    └── game_of_life/   (escribes: *.c *.h)
```

## Cómo se usa

1. Entra en una carpeta de ejercicio y crea **tú** los ficheros que faltan
   (mira `subject.txt` y el `main.cpp`/prototipo que ya está puesto).
2. Lanza el tester:
   ```sh
   cd evaluacion
   ./grademe.sh            # todos
   ./grademe.sh vect2      # uno solo
   ./grademe.sh bigint polyset
   ```
3. El tester **compila tu código con las flags del examen** y compara tu salida,
   byte a byte (`diff`), con la de la solución de referencia generada al vuelo.
   `[OK]` = idéntico. `[KO]` = te muestra el diff (`-` esperado / `+` tuyo).

> Las soluciones viven en `.solutions/` solo para que el tester genere la salida
> esperada en tu máquina. No las abras hasta haber intentado el ejercicio.

> ⚠️ **bsq**: el enunciado avisa de que en el examen actual la primera línea del
> mapa puede ir **sin separación por espacios**. Los mapas de `maps/` y la
> referencia usan el formato clásico con espacios (`9 . o x`). El algoritmo es el
> mismo; si en tu examen cambia el parseo, adapta solo la lectura de la 1ª línea.

---

# REPASO DE CONCEPTOS

## 0. Reglas del examen que SIEMPRE se evalúan

- **Compila con** `-Wall -Wextra -Werror`. En C++ además `-std=c++98`.
  Un solo warning = 0. (En C++98 usa `0`/`NULL`, no `nullptr`, si tu g++ es
  estricto; la referencia usa `nullptr` y compila porque g++ lo tolera, pero
  **para tu código juega seguro con C++98 puro**.)
- **Nada de fugas de memoria.** Todo `new` lleva su `delete`, todo `new[]` su
  `delete[]`, todo `malloc` su `free`.
- **Salida EXACTA.** Un espacio o salto de línea de más y el tester da `[KO]`.
- En C los **allowed functions** son una lista cerrada: usar otra = 0.

## 1. Forma Canónica Ortodoxa (OCF) — la base de todo C++ aquí

Toda clase que gestione recursos debe declarar las **4 grandes**:

```cpp
class Foo {
public:
    Foo();                              // 1. constructor por defecto
    Foo(const Foo& other);              // 2. constructor por copia
    Foo& operator=(const Foo& other);   // 3. operador de asignación
    ~Foo();                             // 4. destructor
};
```

- El **operador de asignación** debe: comprobar autoasignación (`if (this != &other)`),
  liberar lo viejo, copiar lo nuevo, y `return *this;`.
- **Regla de oro**: si haces *deep copy* en el copy-ctor, hazla igual en el `operator=`.
- **`const` correctness**: cualquier método que no modifique el objeto debe ser
  `const` (`int get() const;`). Los getters, `print`, `has`, `operator[] const`,
  `operator==`… todos `const`. El examen lo dice explícitamente en polyset.

## 2. Sobrecarga de operadores (vect2 y bigint)

| Operador | Firma típica | Devuelve | Notas |
|----------|--------------|----------|-------|
| `+ - *`  | `T operator+(const T&) const` | **valor** nuevo | no modifica `*this` |
| `+= -= *=` | `T& operator+=(const T&)` | `*this` por ref | modifica e itera (`a += b += c`) |
| `==` `!=` | `bool operator==(const T&) const` | bool | `!=` suele ser `return !(*this==o)` |
| `< > <= >=` | `bool operator<(const T&) const` | bool | bigint los pide todos |
| `[]` | `int& operator[](int)` **y** `int operator[](int) const` | ref / valor | dos versiones: lectura y escritura |
| `++ --` (pre) | `T& operator++()` | `*this` | incrementa y devuelve |
| `++ --` (post) | `T operator++(int)` | **copia previa** | el `int` es un dummy para distinguir |
| unario `-` | `T operator-() const` | valor | negación |
| `<<` (stream) | `std::ostream& operator<<(std::ostream&, const T&)` | el stream | **NO es miembro**, función libre |
| `*` por escalar conmutativo | `T operator*(int, const T&)` | valor | función libre, para `2 * v` |

Claves que se cuelan en el examen:

- **`operator<<` es función libre**, no método. Si necesita acceder a privados,
  hazla `friend` o usa los getters/`operator[]` públicos.
- **`2 * v`** (escalar a la izquierda) necesita una **función libre**
  `operator*(int, const vect2&)`; el método `v * 2` no cubre ese caso.
- **post-incremento devuelve por valor** una copia del estado anterior; el
  parámetro `int` no se usa, solo distingue de la versión prefijo.
- **`operator[]` doble**: versión no-const devuelve `int&` (permite `v[0]=5`),
  versión `const` devuelve `int` (para objetos const, solo lectura).
- Encadenamiento: `a += b += c` y `cout << x << y` funcionan porque devuelves
  referencia (`*this` / el `ostream&`).

### vect2 concreto
Vector 2D de `int`. Constructores `()`, `(int,int)`, copia, `=`, dtor. `[]` con
0/1 sin comprobar límites. `+ - *` (escalar), versiones `+= -= *=`, `== !=`,
unario `-`, pre/post `++ --`, y `<<` que imprime `{x, y}`. **Todo con `int`,
sin memoria dinámica** → la OCF es trivial pero hay que escribirla igual.

### bigint concreto
Entero sin signo de **precisión arbitraria**. Guarda los dígitos en un
`std::string` (lo más cómodo en C++98) o un vector de dígitos.
- **Suma** (`+`, `+=`): suma como en el cole, de derecha a izquierda, con acarreo.
- **Comparación** (`< > <= >= == !=`): primero por longitud (sin ceros a la
  izquierda), si igual longitud, comparación lexicográfica.
- **Digit-shift**: `<<` multiplica por 10^n (añade n ceros a la derecha);
  `>>` divide por 10^n (quita n dígitos por la derecha). ¡Ojo!: son shifts de
  **dígitos en base 10**, no de bits. El operando del shift puede ser otro `bigint`
  (en el main: `d >>= (const bigint)2`).
- **Impresión sin ceros a la izquierda**. Normaliza siempre tras operar.
- Constructor desde `int` (y `int` grande). `const` en todo lo que no muta.

## 3. Herencia, clases abstractas y herencia virtual (polyset)

Este es el ejercicio "conceptual". Lo que debes dominar:

- **Clase abstracta / método virtual puro**: `virtual void insert(int) = 0;`.
  No se puede instanciar; obliga a las hijas a implementarlo.
- **`bag`** es abstracta (insert, print, clear). **`searchable_bag`** hereda de
  `bag` y añade `virtual bool has(int) const = 0;`.
- **Diamante / herencia virtual**: `array_bag` y `searchable_bag` heredan ambas
  de `bag` con **`virtual public bag`**. Cuando
  `searchable_array_bag : public searchable_bag, public array_bag`
  hereda de las dos, la herencia *virtual* evita tener **dos copias de `bag`**.
  Por eso en las bases verás `class array_bag : virtual public bag`. Sin el
  `virtual`, el diamante no resuelve y no compila.
- **Tu trabajo (parte 1)**: `searchable_array_bag` y `searchable_tree_bag`
  heredan implementación (array/árbol) + interfaz searchable, y solo implementan
  `has(int) const`:
  - array: recorre el array buscando el valor.
  - árbol: búsqueda en BST (recursiva), aprovechando que está ordenado.
  - Cada una con su OCF (ctor, copia, `=`, dtor) llamando a las de la base.
- **Tu trabajo (parte 2) `set`**: envuelve un `searchable_bag&` (una
  **referencia**) y lo convierte en conjunto (no admite duplicados → en `insert`
  comprueba `has()` antes de insertar).
  - **Miembro referencia** ⇒ **no** puede haber `operator=` por defecto (una
    referencia no se reasigna) ni constructor por defecto. El examen los pone
    **privados** (ctor por defecto, copia y `=`) para prohibirlos.
  - `get_bag()` devuelve el bag envuelto; `print`, `clear`, `has`, `insert`
    delegan en el bag.
  - `const` donde toque (`has`, ...).

> Detalle del `main` de polyset: la salida incluye los mensajes de depuración que
> imprime `tree_bag` ("create node", "destroying value", "duplicate value"). Eso
> es **código provisto**, no lo toques: el tester los espera idénticos.

## 4. Conceptos de C (level_2)

### bsq — biggest square
- **Lectura de fichero** (`fopen`/`getline` o `fscanf`) y de **stdin** si no hay
  argumentos. Allowed: malloc/calloc/realloc/free, fopen/fclose, getline, fscanf,
  fputs/fprintf, stderr/stdout/stdin, errno.
- **Parseo de la 1ª línea**: nº de líneas, char vacío, char obstáculo, char lleno.
- **Validación del mapa** → si es inválido: `"map error\n"` a **stderr** y pasa al
  siguiente. Inválido si: líneas de distinta longitud, char no declarado, dos de
  los tres chars iguales, falta info, mapa vacío.
- **Algoritmo (DP)**: por cada celda vacía,
  `size[i][j] = 1 + min(size[i-1][j], size[i][j-1], size[i-1][j-1])`;
  obstáculo ⇒ 0. Guardas el máximo y su posición (el más **arriba**, luego más a
  la **izquierda** en empates) y rellenas ese cuadrado con el char "lleno".
- **Memoria dinámica 2D** y liberarla toda. Cuidado con `\n` finales y el
  separador entre múltiples mapas.

### game_of_life
- `./life width height iterations` (`atoi`). Allowed: atoi, read, putchar,
  malloc, calloc, realloc, free. (¡Sin `printf`! → imprime con `putchar`.)
- **Entrada por stdin con `read`**: comandos `w a s d` mueven un "lápiz",
  `x` baja/sube el lápiz (empieza/para de dibujar). El lápiz **no sale** del
  tablero (si un comando lo sacaría, se queda quieto). Comando inválido = ignora.
  Al dibujar, marca la celda como viva.
- **Tablero**: array (1D con `y*width+x`, o 2D). Celdas fuera del tablero = muertas.
- **Reglas de Conway** por iteración (cuenta vecinos vivos de las 8 direcciones):
  - viva con 2 o 3 vecinos → sigue viva; si no, muere.
  - muerta con exactamente 3 vecinos → nace.
  - Calcula el **nuevo estado en un buffer aparte** y luego cópialo (no mutes
    mientras cuentas).
- **Salida**: celda viva = `'0'` (cero), muerta = espacio `' '`, y `\n` por fila.

## 5. Checklist mental antes de entregar

- [ ] Compila con las flags exactas, **0 warnings**.
- [ ] OCF completa y `const` en su sitio (C++).
- [ ] Sin fugas (`valgrind ./a.out ...` si puedes).
- [ ] Salida byte a byte igual al `main`/ejemplos (usa `cat -e` para ver `\n`/espacios).
- [ ] Casos límite: vacíos, duplicados, mapas inválidos, 0 iteraciones.
- [ ] Solo funciones permitidas (C).

¡Suerte! 🟢
