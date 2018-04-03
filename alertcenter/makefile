TARGET  := alertcenter
INCS = -I ../network -I ../utils
LIBS = -L ../utils -lutils -L ../network -lnetwork -Wl,-Bstatic -lev -lhiredis -ljansson -lmpdec -lssl -lcrypto -Wl,-Bdynamic -lm -lcurl 
include ../makefile.inc
