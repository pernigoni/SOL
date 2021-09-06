gcc tokenizer_lib.c -c -o tokenizer_lib.o

ar rvs libtok.a tokenizer_lib.o

gcc main.c -o prog -L . -ltok


ESEMPI DI ESECUZIONE

./prog 0 "c i a o"

./prog 1 "c i a o"
