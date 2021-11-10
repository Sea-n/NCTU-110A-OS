all:
	gcc -Wall -Wextra -o main main.c
	gcc -Wall -Wextra -fPIC -c ff.c
	gcc -shared -Wl,-soname,ff.so.1 -o ff.so ff.o
