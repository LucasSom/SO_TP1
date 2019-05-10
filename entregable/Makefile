.PHONY: all clean

CC=g++
CPP_STANDARD=c++11
CFLAGS=-c -pthread -std=$(CPP_STANDARD)
LFLAGS=-pthread -std=$(CPP_STANDARD)

OBJS=grafo.o

all: TP1 TP1Spinlock

TP1: $(OBJS) tp1.cpp
	$(CC) $(LFLAGS) $(OBJS) tp1.cpp -o TP1

TP1Spinlock: $(OBJS) tp1Spinlock.cpp
	$(CC) $(LFLAGS) $(OBJS) tp1Spinlock.cpp -o TP1Spinlock

grafo.o: grafo.h grafo.cpp
	$(CC) $(CFLAGS) grafo.cpp -o grafo.o

clean:
	rm $(OBJS)
	rm TP1

test: TP1
	./TP1 $(input)
