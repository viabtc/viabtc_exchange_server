TARGET  := matchengine.exe
PQ =  `/usr/bin/pkg-config --cflags --libs libpq`
INCS = -I ../network -I ../utils $(PQ)
LIBS = -L ../utils -lutils -L ../network -lnetwork -Wl,-Bstatic -lev -ljansson -lmpdec -lmysqlclient -lz -lrdkafka -lssl -lcrypto -lhiredis -Wl,-Bdynamic -lm -lpthread -ldl $(PQ)
include ../makefile.inc
