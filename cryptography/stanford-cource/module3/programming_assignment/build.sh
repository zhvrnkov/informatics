SRC="${1:-main.cc}"
clang++ -O3 -std=c++23 -I./ -L./cryptopp $SRC -lcryptopp -o "${SRC%.*}"
