SOURCE  := $(wildcard *.c)
OBJS    := $(patsubst %.c, %.o, $(SOURCE))
CC      := gcc
LD      := g++ #we will need to link c++ mysql client library
CFLAGS  := -Wall -Wno-strict-aliasing -Wno-uninitialized -g -rdynamic -std=gnu99
LFLAGS  := -g -rdynamic

.PHONY : all clean install

all : $(TARGET)

clean :
	rm -rf *.d *.o $(TARGET)

$(TARGET) : $(OBJS)
	$(LD) $(LFLAGS) -o $@ $(OBJS) $(LIBS)
.c.o :
	$(CC) $(CFLAGS) -c -o $@ $< $(INCS)

install :

