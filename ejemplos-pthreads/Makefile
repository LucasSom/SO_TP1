CC=gcc
CFLAGS=-O0 -pthread
EJECUS=holamundo1 holamundo2 

.PHONY = all clean new

all : $(EJECUS)

clean :
	rm -f $(EJECUS)

new : clean all

mtgz : mtgz.c
	$(CC) $(CFLAGS) $^ -o $@ -lz
