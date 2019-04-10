#include <pthread.h>
#include <stdio.h>

#define  CANT_THREADS  8

void *hola_mundo(void *p_minumero)
{
    int minumero = *((int *) p_minumero);
    printf("Hola mundo! Soy el thread nro. %d.\n", minumero);
    return NULL;
}

int main(int argc, char **argv) 
{
    pthread_t thread[CANT_THREADS]; int tid;

    for (tid = 0; tid < CANT_THREADS; ++tid)
         pthread_create(&thread[tid], NULL, hola_mundo, &tid);

    for (tid = 0; tid < CANT_THREADS; ++tid)
         pthread_join(thread[tid], NULL);

    return 0;
}
