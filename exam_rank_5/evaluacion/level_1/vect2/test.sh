#!/bin/sh
# Tester vect2 : compila tus ficheros + main.cpp y compara con la referencia.
. ../../_lib.sh
REF=../../.solutions/vect2
NEED="vect2.hpp vect2.cpp"

CXX=$(find_cxx) || { fail "no hay compilador C++"; exit 1; }
for f in $NEED; do
	[ -f "$f" ] || { fail "falta el fichero: $f"; exit 1; }
done

TMP=$(mktemp -d)
# 1) Compilar TU version (en su propio dir, con tus headers)
if ! $CXX $CXXFLAGS main.cpp vect2.cpp -o "$TMP/mine" 2>"$TMP/err"; then
	fail "tu codigo NO compila (-Wall -Wextra -Werror -std=c++98)"
	cat "$TMP/err"; rm -rf "$TMP"; exit 1
fi
pass "compila sin warnings"

# 2) Compilar la referencia AISLADA (su main + sus headers)
cp main.cpp "$REF/vect2.hpp" "$REF/vect2.cpp" "$TMP/"
( cd "$TMP" && $CXX $CXXFLAGS main.cpp vect2.cpp -o ref 2>/dev/null )

"$TMP/mine" > "$TMP/out_mine" 2>&1
"$TMP/ref"  > "$TMP/out_ref"  2>&1
diff_check "$TMP/out_ref" "$TMP/out_mine" "vect2"
rm -rf "$TMP"
