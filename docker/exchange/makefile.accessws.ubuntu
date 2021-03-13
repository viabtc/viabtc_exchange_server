TARGET  := accessws
INCS = -I ../network -I ../utils
LIBS = -L ../utils -lutils -L ../network -lnetwork -Wl,-Bstatic -lev -ljansson -lmpdec -lrdkafka -lcurl -lz -lssl -lcrypt -lcrypto -lhiredis -Wl,-Bdynamic -lm -lpthread -ldl -lldap -lkrb5 -llber -lgss -lidn -lrtmp
include ../makefile.inc
