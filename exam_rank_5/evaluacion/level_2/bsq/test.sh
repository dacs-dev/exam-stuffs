#!/bin/sh
# Tester bsq : compila TODOS tus *.c y compara salida con la referencia
# sobre cada mapa de maps/ (incluye mapas invalidos -> "map error").
. ../../_lib.sh
REF=../../.solutions/bsq

CC=$(find_cc) || { fail "no hay compilador C"; exit 1; }
# hay al menos un .c tuyo?
ls ./*.c >/dev/null 2>&1 || { fail "no hay ficheros .c en este directorio"; exit 1; }

TMP=$(mktemp -d)
# 1) Compilar TU version (todos los .c del dir actual)
if ! $CC $CFLAGS ./*.c -o "$TMP/mine" 2>"$TMP/err"; then
	fail "tu codigo NO compila (-Wall -Wextra -Werror)"
	cat "$TMP/err"; rm -rf "$TMP"; exit 1
fi
pass "compila sin warnings"

# 2) Compilar referencia
$CC $CFLAGS "$REF"/*.c -o "$TMP/ref" 2>/dev/null

# 3) Comparar por cada mapa (stdout y stderr juntos: 'map error' va a stderr)
status=0
for map in maps/*.map; do
	"$TMP/mine" "$map" >"$TMP/m.out" 2>&1
	"$TMP/ref"  "$map" >"$TMP/r.out" 2>&1
	if diff -u "$TMP/r.out" "$TMP/m.out" >"$TMP/d" 2>&1; then
		pass "$(basename "$map")"
	else
		fail "$(basename "$map") -> diff:"; head -25 "$TMP/d"; status=1
	fi
done

# 4) Test por stdin (sin argumentos)
"$TMP/mine" < maps/good_01.map >"$TMP/m.out" 2>&1
"$TMP/ref"  < maps/good_01.map >"$TMP/r.out" 2>&1
diff -u "$TMP/r.out" "$TMP/m.out" >/dev/null 2>&1 && pass "stdin (good_01)" || { fail "stdin (good_01)"; status=1; }

# 5) Varios mapas en una sola llamada
"$TMP/mine" maps/good_01.map maps/bad_dupchar.map maps/good_02.map >"$TMP/m.out" 2>&1
"$TMP/ref"  maps/good_01.map maps/bad_dupchar.map maps/good_02.map >"$TMP/r.out" 2>&1
diff -u "$TMP/r.out" "$TMP/m.out" >/dev/null 2>&1 && pass "multi-mapa" || { fail "multi-mapa"; status=1; }

rm -rf "$TMP"
exit $status
