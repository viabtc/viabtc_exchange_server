TARGET  := libutils.a
PQ =  `pkg-config --cflags libpq`
INCS = -I ../network $(PQ)
include ../makefile.inc
$(TARGET) : $(OBJS)
	ar -rs $@ $^
