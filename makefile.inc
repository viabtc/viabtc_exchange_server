SOURCE  := $(wildcard *.c)
OBJS    := $(patsubst %.c, %.o, $(SOURCE))
CC      := gcc
CFLAGS  := -Wall -Wno-strict-aliasing -Wno-uninitialized -g -rdynamic -std=gnu99 -no-pie
LFLAGS  := -g -rdynamic -no-pie


.PHONY : all clean install 

all : $(TARGET)

clean :
	rm -rf *.d *.o $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(LFLAGS) -o $@ $(OBJS) $(LIBS) -lstdc++
.c.o :
	$(CC) $(CFLAGS) -c -o $@ $< $(INCS)

install :

