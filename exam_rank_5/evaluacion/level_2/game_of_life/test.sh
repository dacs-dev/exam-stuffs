#!/bin/sh
# Tester game_of_life : compila tus *.c y compara con la referencia.
# Prototipo:  ./life width height iterations   (comandos w a s d x por stdin)
. ../../_lib.sh
REF=../../.solutions/game_of_life

CC=$(find_cc) || { fail "no hay compilador C"; exit 1; }
ls ./*.c >/dev/null 2>&1 || { fail "no hay ficheros .c en este directorio"; exit 1; }

TMP=$(mktemp -d)
if ! $CC $CFLAGS ./*.c -o "$TMP/mine" 2>"$TMP/err"; then
	fail "tu codigo NO compila (-Wall -Wextra -Werror)"
	cat "$TMP/err"; rm -rf "$TMP"; exit 1
fi
pass "compila sin warnings"
$CC $CFLAGS "$REF"/*.c -o "$TMP/ref" 2>/dev/null

# casos: "input|w|h|iters"
CASES="
sdxddssaaww|5|5|0
sdxssdswdxdddxsaddawxwdxwaa|10|6|0
dxss|3|3|0
sdxddssaaww|5|5|1
sdxddssaaww|5|5|3
dxdxdxdx|6|6|2
|4|4|5
"
status=0
echo "$CASES" | while IFS='|' read -r inp w h it; do
	[ -z "$w" ] && continue
	printf '%s' "$inp" | "$TMP/mine" "$w" "$h" "$it" >"$TMP/m.out" 2>&1
	printf '%s' "$inp" | "$TMP/ref"  "$w" "$h" "$it" >"$TMP/r.out" 2>&1
	if diff -u "$TMP/r.out" "$TMP/m.out" >"$TMP/d" 2>&1; then
		pass "in='$inp' $w $h $it"
	else
		fail "in='$inp' $w $h $it -> diff:"; head -20 "$TMP/d"
	fi
done
rm -rf "$TMP"
