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
	for number in 1 2 3 ; do \
    	./testPar trivial.txt p $$number ; \
	done
	for i in 1 2 3 4 5 6 7 8 9 ; do \
		./testPar simple.txt p $$i ; \
	done
	for j in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 ; do \
		./testPar complejo.txt p $$j ; \
	done

tests-comparativo: $(OBJS) tp1-entrega.cpp
	$(CC) $(LFLAGS) $(OBJS) tp1-entrega.cpp -o testComp
	for i in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99 100 ; do \
		./testComp 100NodosRandom.txt a $$i ; \
	done
	for j in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111 112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127 128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143 144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159 160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175 176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191 192 193 194 195 196 197 198 199 200 ; do \
		./testComp 200NodosCompleto.txt a $$j ; \
	done


grafo.o: grafo.h grafo.cpp
	$(CC) $(CFLAGS) grafo.cpp -o grafo.o

clean:
	rm $(OBJS)
	rm TP1

test: TP1
	./TP1 $(input)
