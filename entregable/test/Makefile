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

tests-secuencial: $(OBJS) tp1-entrega.cpp
	$(CC) $(LFLAGS) $(OBJS) tp1-entrega.cpp -o testSec
	./testSec trivial.txt s 0
	./testSec simple.txt s 0 
	./testSec complejo.txt s 0

tests-paralelo: $(OBJS) tp1-entrega.cpp
	$(CC) $(LFLAGS) $(OBJS) tp1-entrega.cpp -o testPar
	./testPar trivial.txt p 1
	./testPar trivial.txt p 2
	./testPar trivial.txt p 3
	./testPar simple.txt p 1
	./testPar simple.txt p 2
	./testPar simple.txt p 5
	./testPar simple.txt p 9
	./testPar complejo.txt p 1
	./testPar complejo.txt p 2
	./testPar complejo.txt p 5
	./testPar complejo.txt p 10
	./testPar complejo.txt p 20

tests-comparativo: $(OBJS) tp1-entrega.cpp
	$(CC) $(LFLAGS) $(OBJS) tp1-entrega.cpp -o testComp
	./testComp 100NodosRandom.txt a 1
	./testComp 100NodosRandom.txt a 20
	./testComp 100NodosRandom.txt a 50
	./testComp 100NodosRandom.txt a 100
	./testComp 200NodosCompleto.txt a 1
	./testComp 200NodosCompleto.txt a 20
	./testComp 200NodosCompleto.txt a 50
	./testComp 200NodosCompleto.txt a 100
	./testComp 200NodosCompleto.txt a 200


grafo.o: grafo.h grafo.cpp
	$(CC) $(CFLAGS) grafo.cpp -o grafo.o

clean:
	rm $(OBJS)
	rm TP1

test: TP1
	./TP1 $(input)
