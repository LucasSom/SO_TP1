# SO_TP1

En el directorio entregable/ correr `make` y para ejecutar:
* `./TP1 <nombre_grafo_a_correr.txt> <cantidad_de_threads>`
* `./TP1Spinlock <nombre_grafo_a_correr.txt> <cantidad_de_threads>` para correr la versión con spinlocks

## Tests
Para ejecutar los tests ir al directorio entregable/test y ejecutar (los tests se corren solos):
* `make tests-secuencial` para correr los tests para el algoritmo secuencial
* `make tests-paralelo` para correr los tests para el algoritmo paralelo
* `make tests-comparativo` correr los tests para el algoritmo paralelo comparando con el resultado del secuencial