# shellcheck shell=sh
# Helpers compartidos por los testers. Se incluye con:  . ../../_lib.sh
GREEN='\033[0;32m'; RED='\033[0;31m'; YEL='\033[1;33m'; NC='\033[0m'

# Detecta un compilador C++ (el evaluador usa 'c++')
find_cxx () {
	for c in c++ g++ clang++; do
		if command -v "$c" >/dev/null 2>&1; then echo "$c"; return 0; fi
	done
	return 1
}
# Detecta un compilador C
find_cc () {
	for c in cc gcc clang; do
		if command -v "$c" >/dev/null 2>&1; then echo "$c"; return 0; fi
	done
	return 1
}

CXXFLAGS="-Wall -Wextra -Werror -std=c++98"
CFLAGS="-Wall -Wextra -Werror"

pass () { printf "${GREEN}[OK]   ${NC}%s\n" "$1"; }
fail () { printf "${RED}[KO]   ${NC}%s\n" "$1"; }
info () { printf "${YEL}[..]   ${NC}%s\n" "$1"; }

# compara dos ficheros, imprime diff resumido si difieren
# uso: diff_check <esperado> <obtenido> <titulo>
diff_check () {
	if diff -u "$1" "$2" >/tmp/_d.$$ 2>&1; then
		pass "$3 : salida identica"
		rm -f /tmp/_d.$$
		return 0
	else
		fail "$3 : salida DIFERENTE"
		echo "----- diff (esperado=- / tuyo=+) -----"
		head -40 /tmp/_d.$$
		echo "--------------------------------------"
		rm -f /tmp/_d.$$
		return 1
	fi
}
