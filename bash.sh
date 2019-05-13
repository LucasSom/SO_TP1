make
mkdir results
./TP1 test/simple.txt 2 > nuevosOut/simple.out
./TP1 test/100NodosRandom.txt 2 > nuevosOut/100NodosRandom.out
./TP1 test/200NodosCompleto.txt 2 > nuevosOut/200NodosCompleto.out
./TP1 test/MilNodos06PRandom.txt 2 > nuevosOut/MilNodosRandom.out

./TP1Spinlock test/simple.txt 2 > nuevosOut/simpleSpinlock.out
./TP1Spinlock test/100NodosRandom.txt 2 > nuevosOut/100NodosRandomSpinlock.out
./TP1Spinlock test/200NodosCompleto.txt 2 > nuevosOut/200NodosCompletoSpinlock.out
./TP1Spinlock test/MilNodos06PRandom.txt 2 > nuevosOut/MilNodosRandomSpinlock.out