#!/bin/sh
# Tester bigint : compila tus ficheros + main.cpp y compara con la referencia.
. ../../_lib.sh
REF=../../.solutions/bigint
NEED="bigint.hpp bigint.cpp"

CXX=$(find_cxx) || { fail "no hay compilador C++"; exit 1; }
for f in $NEED; do
	[ -f "$f" ] || { fail "falta el fichero: $f"; exit 1; }
done

TMP=$(mktemp -d)
if ! $CXX $CXXFLAGS main.cpp bigint.cpp -o "$TMP/mine" 2>"$TMP/err"; then
	fail "tu codigo NO compila (-Wall -Wextra -Werror -std=c++98)"
	cat "$TMP/err"; rm -rf "$TMP"; exit 1
fi
pass "compila sin warnings"

cp main.cpp "$REF/bigint.hpp" "$REF/bigint.cpp" "$TMP/"
( cd "$TMP" && $CXX $CXXFLAGS main.cpp bigint.cpp -o ref 2>/dev/null )

"$TMP/mine" > "$TMP/out_mine" 2>&1
"$TMP/ref"  > "$TMP/out_ref"  2>&1
diff_check "$TMP/out_ref" "$TMP/out_mine" "bigint"
rm -rf "$TMP"
