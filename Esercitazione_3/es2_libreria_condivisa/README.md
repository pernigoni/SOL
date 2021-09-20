`gcc tokenizer_lib.c -c -fPIC -o tokenizer_lib.o`

`gcc -shared -o libtok.so tokenizer_lib.o`

`gcc main.c -o prog -L . -ltok`


ESEMPI DI ESECUZIONE

`LD_LIBRARY_PATH="." ./prog 0 "c i a o"`

`LD_LIBRARY_PATH="." ./prog 1 "c i a o"`
