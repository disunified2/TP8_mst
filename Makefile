build: mst

all: indent build test

indent:
	sed "s/\r//g" -i *.h *.c
	indent -npsl -nut *.h *.c

mst: mst.h mst.c mst_tests.c
	gcc -std=c99 -Wall -Wextra -pedantic -g -O2 $@_tests.c $@.c -o $@

test: mst
	valgrind -q --leak-check=full ./$^

clean:
	rm -f mst *~
