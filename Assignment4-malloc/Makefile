all:
	gcc -Wall -Wextra -o main main.c
	gcc -Wall -Wextra -fPIC -c ff.c
	gcc -shared -Wl,-soname,ff.so.1 -o ff.so ff.o
	gcc -Wall -Wextra -fPIC -c bf.c
	gcc -shared -Wl,-soname,bf.so.1 -o bf.so bf.o
