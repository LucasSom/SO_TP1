#include "grafo.h"
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <limits>
#include <mutex>
#include <queue>
#include <utility>
#include <semaphore.h>
#include <unistd.h>
#include <atomic>

using namespace std;

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//////////////// DATOS DEL SECUENCIAL ///////////////////////////
/////////////////////////////////////////////////////////////////

#define BLANCO -10
#define GRIS -20
#define NEGRO -30
#define SHARED_SEM 1

//COLORES de los nodos
vector<int> colores;

//DISTANCIA mínima del nodo al árbol
vector<int> distancia;
vector<int> distanciaNodo;

/* MAXIMO VALOR DE INT */
int IMAX = numeric_limits<int>::max();


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//////////////// DATOS DEL PARALELO /////////////////////////////
/////////////////////////////////////////////////////////////////


//el colores usamos el secuencial que total tenemos un solo arbol compartido
//el IMAX usamos el secuencial

//DISTANCIA mínima del nodo al árbol.
//distancia[i] = vector con todas las dists al arbol del thread i
vector<vector <int> > distanciaParal;
vector<vector <int> > distanciaNodoParal;
// Colores de los nodos para arbol del thread i
vector<vector <int> > coloresArbol;


typedef struct mergeStruct {
  sem_t semDeCola;
  sem_t semDePedidor;
  int tidDelQuePide;

} mergeStruct;

//colasEspera[i] es la cola en la que el thread i ve si alguien pidió un merge
//colasEspera[i] también tiene un mutex para que la cola no se rompa por la concurrencia
vector< pair<mutex, queue<mergeStruct*> > >* colasEspera;

//para modificar el nodo i del grafo compartido hay que pedir permiso
//al mutex permisoNodo[i]
//es un puntero al vector para poder tener tamaño dinámico sin copiar mutexes
vector<mutex>* permisoNodo;

//el arbol resultado que solo modifica el thread que no le quedan
//nodos que agregar (o sea que es el último)
// arbolRta para devolver resultado en paralelo
// grafoCompartido grafo original
Grafo* arbolRta;
Grafo* grafoCompartido;
atomic <int> threadsVivos;
vector<Grafo> arbolesGenerados;

typedef struct inputThread {
  int threadId;
  int nodoInicialRandom;

} inputThread;



/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////



//Pinto el nodo de negro para marcar que fue puesto en el árbol
void pintarNodo(int num){
  colores[num] = NEGRO;
  //Para no volver a elegirlo
  distancia[num] = IMAX;
}

//Pinto los vecinos de gris para marcar que son alcanzables desde el árbol (salvo los que ya son del árbol)
void pintarVecinos(Grafo *g, int num){
  for(vector<Eje>::iterator v = g->vecinosBegin(num); v != g->vecinosEnd(num); ++v){
	//Es la primera vez que descubro el nodo
	if(colores[v->nodoDestino] == BLANCO){
		//Lo marco como accesible
		colores[v->nodoDestino] = GRIS;
		//Le pongo el peso desde donde lo puedo acceder
		distancia[v->nodoDestino] = v->peso;
		//Guardo que nodo me garantiza esa distancia
		distanciaNodo[v->nodoDestino] = num;
	}else if(colores[v->nodoDestino] == GRIS){
		//Si ya era accesible, veo si encontré un camino más corto
		distancia[v->nodoDestino] = min(v->peso,distancia[v->nodoDestino]);
		//Guardo que nodo me garantiza esa distancia
		if(distancia[v->nodoDestino] == v->peso)
		distanciaNodo[v->nodoDestino] = num;
	}
  }
}

void mstSecuencial(Grafo *g){
  //Imprimo el grafo
  g->imprimirGrafo();

  //INICIALIZO
  //Semilla random
  srand(time(0));
  //Arbol resultado
  Grafo arbol;
  //Por ahora no colorié ninguno de los nodos
  colores.assign(g->numVertices,BLANCO);
  //Por ahora no calculé ninguna distancia
  distancia.assign(g->numVertices,IMAX);
  distanciaNodo.assign(g->numVertices,-1);

  //Selecciono un nodo al azar del grafo para empezar
  int nodoActual = rand() % g->numVertices;
  //int nodoActual = 0;
  for(int i = 0; i < g->numVertices; i++){
	arbol.numVertices += 1;

	//La primera vez no lo agrego porque necesito dos nodos para unir
	if(i > 0){
	  arbol.insertarEje(nodoActual,distanciaNodo[nodoActual],distancia[nodoActual]);
	}

	//Lo pinto de NEGRO para marcar que lo agregué al árbol y borro la distancia
	pintarNodo(nodoActual);

	//Descubrir vecinos: los pinto y calculo distancias
	pintarVecinos(g,nodoActual);

	//Busco el nodo más cercano que no esté en el árbol, pero sea alcanzable
	nodoActual = min_element(distancia.begin(),distancia.end()) - distancia.begin();
  }

  cout << endl << "== RESULTADO == " << endl;
  arbol.imprimirGrafo();
}

/////////////////////////////////////////////////////////////////7
//////////////////////////////////////////////////////////////////
//////////////////NUESTRO CODIGO//////////////////////////////////
/////////////////////////////////////////////////////////////////

//Pinto el nodo de (-i) para marcar que fue puesto en el árbol i
void pintarNodoPararelo(int nodo, int miTid){
	colores[nodo] = miTid;
	coloresArbol[miTid][nodo] = NEGRO;
	//Para no volver a elegirlo (desde el thread i)
	distanciaParal[miTid][nodo] = IMAX;
}

void pintarNodoParareloAux(int nodo, int miTid){
	colores[nodo] = miTid;
	coloresArbol[miTid][nodo] = NEGRO;
}

void pintarVecinosParalelo(Grafo *miArbol, int num, int miTid){
  for(vector<Eje>::iterator v = grafoCompartido->vecinosBegin(num); v != grafoCompartido->vecinosEnd(num); ++v){
	//Es la primera vez que descubro el nodo
	if(coloresArbol[miTid][v->nodoDestino] == BLANCO){
		//Lo marco como accesible
		coloresArbol[miTid][v->nodoDestino] = GRIS;
		//Le pongo el peso desde donde lo puedo acceder
		distanciaParal[miTid][v->nodoDestino] = v->peso;
		//Guardo que nodo me garantiza esa distancia
		distanciaNodoParal[miTid][v->nodoDestino] = num;
	}else if(coloresArbol[miTid][v->nodoDestino] == GRIS){
		//Si ya era accesible, veo si encontré un camino más corto
		distanciaParal[miTid][v->nodoDestino] = min(v->peso,distanciaParal[miTid][v->nodoDestino]);
		//Guardo que nodo me garantiza esa distancia
		if(distanciaParal[miTid][v->nodoDestino] == v->peso)
		distanciaNodoParal[miTid][v->nodoDestino] = num;
	}
  }
}

void actualizarLogicaColores(int miTid, int tidAMorir){

	for (int nodo = 0; nodo < coloresArbol[miTid].size(); ++nodo){
		if(coloresArbol[miTid][nodo] != NEGRO){
			if (coloresArbol[tidAMorir][nodo] == NEGRO){
				/*En aMorir es negro pero en el original no.
				Entonces, tengo que pintarlo de negro y actualizar la distancia
				estos van a ser los nodos que agregamos antes del arbolAMorir al arbol que vive*/
				distanciaParal[miTid][nodo] = distanciaParal[tidAMorir][nodo];//distancia va a ser IMAX
				distanciaNodoParal[miTid][nodo] = distanciaNodoParal[tidAMorir][nodo];
				coloresArbol[miTid][nodo] = NEGRO;

				permisoNodo->operator[](nodo).lock();
				colores[nodo] = miTid;
				permisoNodo->operator[](nodo).unlock();
			}else{//ambos son blancos o grises
				if (coloresArbol[tidAMorir][nodo] == GRIS){
					if (coloresArbol[miTid][nodo] == GRIS){
						/* Ambos son grises. Voy a querer guardar la menor distancia
						para que luego se use la distancia efectivamente menor*/
						if (distanciaParal[miTid][nodo] > distanciaParal[tidAMorir][nodo]){
							distanciaParal[miTid][nodo] = distanciaParal[tidAMorir][nodo];
							distanciaNodoParal[miTid][nodo] = distanciaNodoParal[tidAMorir][nodo];
							//Ya esta de gris, no tengo que actualizar el color
						}//ELSE: dejo como estaba (si ya tenía la distancia menor)
					}else{
						/*En el original es blanco, y en el aMorir, gris*/
						distanciaParal[miTid][nodo] = distanciaParal[tidAMorir][nodo];
						distanciaNodoParal[miTid][nodo] = distanciaNodoParal[tidAMorir][nodo];
						coloresArbol[miTid][nodo] = GRIS;
					}
				}//ELSE: si el nodo en el arbol de aMorir es blanco, dejo lo que esta
			}
		}//ELSE: si el nodo en el arbol del thread que vive es negro, no tengo que hacer nada
			//(porque ese nodo ya está incluído en el arbol que sobrevive)
	}

}

void sumar_arbol(int tidDelQuePide, int tidCola, int nodoActual, bool esPrimerNodo){
	int tidAMorir, miTid;
	Grafo* original;
	Grafo* aMorir;
	if (tidDelQuePide < tidCola){
		miTid = tidDelQuePide;
		tidAMorir = tidCola;
	}else{
		miTid = tidCola;
		tidAMorir = tidDelQuePide;
	}
	original = &arbolesGenerados[miTid];
	aMorir = &arbolesGenerados[tidAMorir];

	//al arbol privado del thread que va a sobrevivir le agregamos todos los ejes y nodos
	//del arbol privado que va a morir. Como hasta ahora no compartían nodos sabemos que los
	//arboles son disjuntos entonces simplemente agregamos todo
	for (map<int,vector<Eje>>::iterator nodo_ptr = aMorir->listaDeAdyacencias.begin(); nodo_ptr != aMorir->listaDeAdyacencias.end(); ++nodo_ptr){
		//recorre todos los nodos del arbol aMorir
		for (vector<Eje>::iterator it = aMorir->vecinosBegin(nodo_ptr->first); it != aMorir->vecinosEnd(nodo_ptr->first); ++it){	
			//recorre los ejes de cada nodo
			if (nodo_ptr->first < it->nodoDestino){
			//para evitar push_backear dos veces cada eje, solo inserto al menor
				original->insertarEje(nodo_ptr->first, it->nodoDestino, it->peso);
			}
		}
	}
	original->numVertices += aMorir->numVertices;
	if (!esPrimerNodo){
		if(distanciaParal[tidAMorir][nodoActual] < distanciaParal[miTid][nodoActual]){
			distanciaParal[miTid][nodoActual] = distanciaParal[tidAMorir][nodoActual];
			distanciaNodoParal[miTid][nodoActual] = distanciaNodoParal[tidAMorir][nodoActual];
		}
		original->insertarEje(nodoActual,distanciaNodoParal[miTid][nodoActual],distanciaParal[miTid][nodoActual]);
	}
	//cada thread tiene su vector de colores (BLANCO,GRIS,NEGRO) para saber cuales distancias
	//ya calculó, como el secuencial. Copiamos todos los colores y distancias del thread que muere
	//al otro
	actualizarLogicaColores(miTid, tidAMorir);

	//ahora copiamos la cola de espera del thread que va a morir, por si tenía 
	//a alguien esperando para mergearse, se lo pasa al que se lo comió

	colasEspera->operator[](tidDelQuePide).first.lock();

	while (! colasEspera->operator[](tidAMorir).second.empty()){
		mergeStruct* encolado = colasEspera->operator[](tidAMorir).second.front();
		colasEspera->operator[](tidAMorir).second.pop();
		colasEspera->operator[](miTid).second.push(encolado);
	}		

	colasEspera->operator[](tidDelQuePide).first.unlock();
	//desbloqueamos esto aunque en teoría nunca lo volvamos a usar
}

void chequeoColaPorPedidos(int miTid){

	while(!(colasEspera->operator[](miTid).second.empty())){
		//si tengo algo en la cola me mergeo

		//agarro los dos mutex que me pasó el primer thread con el que me voy a mergear (y su tid)
		mergeStruct* rendezvousP = colasEspera->operator[](miTid).second.front(); 
		colasEspera->operator[](miTid).second.pop();
		printf("Soy cola %d con pedidor %d\n", miTid, rendezvousP->tidDelQuePide);
		//hago rendezvous para que el otro thread (o yo) no muera hasta que me mergee con el
		sem_post(&rendezvousP->semDeCola);
		sem_wait(&rendezvousP->semDePedidor);
		//se que en este momento el thread que me lo pidió está haciendo el merge, porque
		//estamos en el rendezvous
		printf("LLegue merge desde cola %d\n", miTid);
		sem_post(&rendezvousP->semDeCola);
		sem_wait(&rendezvousP->semDePedidor);
		printf("Pase merge desde cola %d\n", miTid);
		//rendezvous.tidDelQuePide es el Tid del thread que se unió conmigo
		if(rendezvousP->tidDelQuePide < miTid){
			//en este caso muero
			colasEspera->operator[](miTid).first.unlock();
			//se supone que esta cola no se vuelve a tocar igual, porque ya no hay nodos con este color, pero por las dudas
			threadsVivos--;
			pthread_exit(NULL);
		}
	}
}

void *ThreadCicle(void* inThread){

	//cada thread tiene su input con tu Tid y su nodo inicial
	inputThread input = *((inputThread *) inThread);

	//agarro el nodo ya elegido al azar que me pasaron
	int nodoActual = input.nodoInicialRandom;
	int miTid = input.threadId;

	//Arbol propio
	Grafo* arbolMio = &(arbolesGenerados[miTid]);

	for(int i = 0; i < grafoCompartido->numVertices; i++){

		//me aseguro que nadie esté tocando este nodo compartido
		permisoNodo->operator[](nodoActual).lock();
		//si ya estaba pintado me mergeo
		if(colores[nodoActual]!=BLANCO){

			int otroThread = colores[nodoActual];

			mergeStruct rendezvous;
			rendezvous.tidDelQuePide = miTid;
			//lockeamos los mutex para que funcione el rendezvous, si no pasan de largo
			//es como inicializar un semaforo en cero

			sem_init(&rendezvous.semDeCola, SHARED_SEM, 0);
			sem_init(&rendezvous.semDePedidor, SHARED_SEM, 0);

			//le aviso al thread "colores[nodoActual]" que estoy esperando para mergearme
			colasEspera->operator[](otroThread).first.lock();
			colasEspera->operator[](otroThread).second.push(&rendezvous);			
			colasEspera->operator[](otroThread).first.unlock();
			
			//ahora que ya le dije al thread que me quiero mergear, suelto el nodo del grafo compartido
			permisoNodo->operator[](nodoActual).unlock();
			printf("Soy pedidor %d cola %d \n", miTid, otroThread);
			sem_post(&rendezvous.semDePedidor);
			sem_wait(&rendezvous.semDeCola);
			printf("LLegue a merge siendo pedidor %d\n", miTid);
			bool esPrimerNodo = i == 0;
			//me mergeo con el thread "colores[nodoActual]"
			//HAGO EL MERGE
			//void sumar_arbol(tidDelQuePideMerge, tidDeCola, nodoCompartido, esPrimerNodo?) es la función merge
			sumar_arbol(miTid, otroThread, nodoActual, esPrimerNodo);
			printf("Paso merge siendo pedidor %d\n", miTid);
			sem_post(&rendezvous.semDePedidor);
			sem_wait(&rendezvous.semDeCola);
			if (otroThread < miTid){
				// HABRIA QUE HACER ALGO MAS ACA??
				threadsVivos--;
				pthread_exit(NULL);
			}

		}else{	
			//este camino es si todo sale bien como el secuencial

			//Lo pinto de NEGRO para marcar que lo agregué al árbol y borro la distancia
			pintarNodoParareloAux(nodoActual, miTid);			
			permisoNodo->operator[](nodoActual).unlock();

			arbolMio->numVertices += 1;

			//La primera vez no lo agrego porque necesito dos nodos para unir
			if(i > 0){
				arbolMio->insertarEje(nodoActual,distanciaNodoParal[miTid][nodoActual],distanciaParal[miTid][nodoActual]);
			}

			distanciaParal[miTid][nodoActual] = IMAX;
			
			//Descubrir vecinos: los pinto y calculo distancias
			pintarVecinosParalelo(arbolMio,nodoActual, miTid);
		}

		//el thread "miTid" chequea su cola a ver si alguien se quiere mergear
		//if (arbolMio->numVertices == 8) sleep(1); 
		colasEspera->operator[](miTid).first.lock();
		chequeoColaPorPedidos(miTid);
		colasEspera->operator[](miTid).first.unlock();

		//me fijo si ya terminé de armar el arbol generador mínimo
		if (arbolMio->numVertices == grafoCompartido->numVertices){
			printf("TERMINEEEEEEEE \n");
			while(threadsVivos > 1){
				colasEspera->operator[](miTid).first.lock();
				chequeoColaPorPedidos(miTid);
				colasEspera->operator[](miTid).first.unlock();
			}
			arbolRta = arbolMio;
			pthread_exit(NULL);
		}

		//tanto si mergee como si no, tengo que buscar el prox nodoActual
		//Busco el nodo más cercano que no esté en el árbol, pero sea alcanzable
		nodoActual = min_element(distanciaParal[miTid].begin(),distanciaParal[miTid].end()) - distanciaParal[miTid].begin();
	}

	//TO DO, creo que nunca va a llegar acá, porque en realidad ningún thread "agrega a mano" los n nodos, porque muchos
	//los consigue por merge, entonces hay que agregar un break adentro del for si no le quedan nodos que agregar
	// Si llegue aca, es el resultado (unico thread que queda)
	arbolRta = arbolMio;
	cout << "llegue al final" << endl;
	return NULL;

}



void mstParalelo(Grafo *g, int cantThreads) {
	//Imprimo el grafo
	g->imprimirGrafo();

	//Semilla random
	srand(time(0));

	// Arreglo para crear threads
	pthread_t thread[cantThreads];

	vector<mutex> mutexeses(g->numVertices);
	permisoNodo = &mutexeses;
	grafoCompartido = g;
	threadsVivos = cantThreads;
	vector<pair<mutex, queue<mergeStruct*> > > mutexesesCola(cantThreads);
	colasEspera = &mutexesesCola;

	//Por ahora no colorié ninguno de los nodos
	colores.assign(g->numVertices,BLANCO);
	coloresArbol.assign(cantThreads, vector<int>(g->numVertices,BLANCO));
	//Por ahora no calculé ninguna distancia
	//en distancia[i] tengo vector de todas las dists al arbol i
	distanciaParal.assign(cantThreads, vector<int>(g->numVertices,IMAX));
	distanciaNodoParal.assign(cantThreads, vector<int>(g->numVertices,-1));

	//estas son las structs que le pasamos a cada thread
	vector<inputThread> inputs(cantThreads);
	//inputs.assign(cantThreads, defecto); //le hacemos un constructor por defecto a la struct?

	//sabemos que queremos un arbol para que genere cada thread
	arbolesGenerados.assign(cantThreads, Grafo());
	//LANZAR LOS THREADS
	for (int tid = 0; tid < cantThreads; ++tid){

		//le pasamos a cada thread su numero de hijo(no el posta del SO, el nuestro)
		inputs[tid].threadId = tid;
		inputs[tid].nodoInicialRandom = rand() % g->numVertices;

        pthread_create(&thread[tid], NULL, &ThreadCicle, &(inputs[tid]));

    }

	//ESPERAR A QUE LOS THREADS MUERAN
    for (int x = 0; x < cantThreads; ++x){
        pthread_join(thread[x], NULL);
    }
	//podríamos agregar que imprima el tiempo que tardó para medirlo,
	//o que lo guarde en algún archivo junto con el tamaño del grafo
	//y cosas así

	//el último thread que queda lo sabe porque no le quedan nodos que agregar
	// guarda su arbol en un arbolRta compartido
	cout << endl << "== RESULTADO == " << endl;
	cout << "Peso total = " << arbolRta->pesoTotal() << endl;
 	arbolRta->imprimirGrafo();
}

/////////////////////////////////////////////////////////////////7
//////////////////////////////////////////////////////////////////
//////////////////NUESTRO CODIGO//////////////////////////////////
/////////////////////////////////////////////////////////////////

int main(int argc, char const * argv[]) {

  if(argc <= 2){
	cerr << "Ingrese nombre de archivo y cantidad de threads" << endl;
	return 0;
  }

  string nombre;
  nombre = string(argv[1]);
  int cantThreads = atoi(argv[2]);

  Grafo g;

  if(cantThreads==1){  	
	  if( g.inicializar(nombre) == 1){

	  	//TO DO, para los tests igual creo que estaría bueno justamente compara el paralelo(g,1) con el secuencial(g)
	  	//o comparar con el secuencial, no siempre con el paralelo

		//Corro el algoirtmo secuencial de g
		mstParalelo(&g, cantThreads);
		//mstSecuencial(&g);

	  }else{
		cerr << "No se pudo cargar el grafo correctamente" << endl;
	  }
  }else{	
	  if( g.inicializar(nombre) == 1){

		//Corro el algoirtmo secuencial de g
		mstParalelo(&g, cantThreads);

	  }else{
		cerr << "No se pudo cargar el grafo correctamente" << endl;
	  }
  }

  return 1;
}





