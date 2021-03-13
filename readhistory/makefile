TARGET  := readhistory
INCS = -I ../network -I ../utils
LIBS = -L ../utils -lutils -L ../network -lnetwork -Wl,-Bstatic -lev -ljansson -lmpdec -lz -lhiredis -Wl,-Bdynamic -lm -lpthread -ldl -lmysqlclient 
include ../makefile.inc
