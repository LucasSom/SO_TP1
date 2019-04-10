#include <pthread.h>
#include <stdio.h>

void *hola_mundo(void *vargp)
{
	printf("Hola mundo!\n");
	return NULL;
}

int main() {
	pthread_t tid;
	
	pthread_create(&tid, NULL, hola_mundo, NULL);
	pthread_join(tid, NULL);
	
	return 0;
}
