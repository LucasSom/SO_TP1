.PHONY: all clean

CC=g++
CPP_STANDARD=c++11
CFLAGS=-c -pthread -std=$(CPP_STANDARD)
LFLAGS=-pthread -std=$(CPP_STANDARD)

OBJS=grafo.o

all: TP1

TP1: $(OBJS) tp1.cpp
	$(CC) $(LFLAGS) $(OBJS) tp1.cpp -o TP1

tests-secuencial: $(OBJS) tp1-entrega.cpp
	$(CC) $(LFLAGS) $(OBJS) tp1-entrega.cpp -o testSec
	./testSec simple.txt s 0

tests-paralelo: $(OBJS) tp1-entrega.cpp
	$(CC) $(LFLAGS) $(OBJS) tp1-entrega.cpp -o testPar
	./testPar simple.txt 3

tests-comparativo: $(OBJS) tp1-entrega.cpp
	$(CC) $(LFLAGS) $(OBJS) tp1-entrega.cpp -o testComp
	./testComp simple.txt 3

grafo.o: grafo.h grafo.cpp
	$(CC) $(CFLAGS) grafo.cpp -o grafo.o

clean:
	rm $(OBJS)
	rm TP1

test: TP1
	./TP1 $(input)
