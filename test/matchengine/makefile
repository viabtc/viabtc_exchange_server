all:
	gcc -o cli.exe -g -std=gnu99 cli.c -I ../../network -I ../../utils -L ../../utils -lutils -L ../../network -lnetwork -lev -ljansson -lmpdec -lm

clearn:
	rm -f cli.exe
