#!/bin/sh
# Tester polyset : compila base + TUS ficheros y compara con la referencia.
# Ficheros a implementar:
#   searchable_array_bag.hpp/.cpp  searchable_tree_bag.hpp/.cpp  set.hpp/.cpp
. ../../_lib.sh
REF=../../.solutions/polyset
NEED="searchable_array_bag.hpp searchable_array_bag.cpp \
      searchable_tree_bag.hpp searchable_tree_bag.cpp set.hpp set.cpp"
BASE="bag.hpp searchable_bag.hpp array_bag.hpp array_bag.cpp tree_bag.hpp tree_bag.cpp main.cpp"
SRCS="main.cpp array_bag.cpp tree_bag.cpp searchable_array_bag.cpp searchable_tree_bag.cpp set.cpp"

CXX=$(find_cxx) || { fail "no hay compilador C++"; exit 1; }
for f in $NEED; do
	[ -f "$f" ] || { fail "falta el fichero: $f"; exit 1; }
done

TMP=$(mktemp -d)
# 1) Compilar TU version
if ! $CXX $CXXFLAGS $SRCS -o "$TMP/mine" 2>"$TMP/err"; then
	fail "tu codigo NO compila (-Wall -Wextra -Werror -std=c++98)"
	cat "$TMP/err"; rm -rf "$TMP"; exit 1
fi
pass "compila sin warnings"

# 2) Compilar referencia aislada (base provista + ficheros de referencia)
mkdir "$TMP/ref"
cp $BASE "$TMP/ref/"
cp "$REF/"*.hpp "$REF/"*.cpp "$TMP/ref/"
( cd "$TMP/ref" && $CXX $CXXFLAGS $SRCS -o ref 2>/dev/null )

# 3) Probar varios juegos de argumentos
status=0
for args in "5 3 8 1 9" "42 42 1 2 2 3" "10" "7 3 9 3 1 5 8"; do
	"$TMP/mine"     $args > "$TMP/m.out" 2>&1
	"$TMP/ref/ref"  $args > "$TMP/r.out" 2>&1
	if diff -u "$TMP/r.out" "$TMP/m.out" >"$TMP/d" 2>&1; then
		pass "args [$args]"
	else
		fail "args [$args] -> diff:"; head -30 "$TMP/d"; status=1
	fi
done
rm -rf "$TMP"
exit $status
