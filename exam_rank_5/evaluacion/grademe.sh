#!/bin/sh
# ============================================================================
#  grademe.sh  -  Tester maestro para el examen (exam_rank_5 / CPP module)
# ----------------------------------------------------------------------------
#  Uso:
#     ./grademe.sh             -> ejecuta TODOS los testers
#     ./grademe.sh vect2       -> solo un ejercicio
#     ./grademe.sh bigint polyset
#
#  Cada ejercicio tiene su propio test.sh dentro de su carpeta.
#  La salida esperada se genera AL VUELO compilando la solucion de
#  referencia oculta en .solutions/<ej>/  y se compara (diff) con la tuya.
#  Asi nunca ves la solucion salvo que abras .solutions/ a proposito.
# ============================================================================
cd "$(dirname "$0")" || exit 1

GREEN='\033[0;32m'; RED='\033[0;31m'; YEL='\033[1;33m'; NC='\033[0m'

EXOS="level_1/vect2 level_1/bigint level_1/polyset level_2/bsq level_2/game_of_life"

run_one () {
	dir="$1"
	if [ ! -f "$dir/test.sh" ]; then
		printf "${YEL}[skip]${NC} %s (sin test.sh)\n" "$dir"
		return
	fi
	printf "\n${YEL}==== %s ====${NC}\n" "$dir"
	( cd "$dir" && sh test.sh )
}

if [ $# -eq 0 ]; then
	for e in $EXOS; do run_one "$e"; done
else
	for arg in "$@"; do
		case "$arg" in
			vect2)        run_one level_1/vect2 ;;
			bigint)       run_one level_1/bigint ;;
			polyset)      run_one level_1/polyset ;;
			bsq)          run_one level_2/bsq ;;
			game_of_life|gol) run_one level_2/game_of_life ;;
			*) printf "${RED}desconocido:${NC} %s\n" "$arg" ;;
		esac
	done
fi
