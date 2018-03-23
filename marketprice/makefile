TARGET  := marketprice.exe
INCS = -I ../network -I ../utils
LIBS = -L ../utils -lutils -L ../network -lnetwork -Wl,-Bstatic -lev -ljansson -lmpdec -lrdkafka -lz -llz4 -lssl -lcrypto -lhiredis -Wl,-Bdynamic -lm -lpthread -ldl -lcurl -lstdc++
include ../makefile.inc
