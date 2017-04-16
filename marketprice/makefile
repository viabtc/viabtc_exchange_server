TARGET  := marketprice.exe
INCS = -I ../network -I ../utils
LIBS = -L ../utils -lutils -L ../network -lnetwork -Wl,-Bstatic -lev -ljansson -lmpdec -lrdkafka -lssl -lcrypto -Wl,-Bdynamic -lm -lpthread
include ../makefile.inc
