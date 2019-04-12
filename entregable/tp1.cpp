#include "grafo.h"
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <limits>

using namespace std;

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//////////////// DATOS DEL SECUENCIAL ///////////////////////////
/////////////////////////////////////////////////////////////////

#define BLANCO -10
#define GRIS -20
#define NEGRO -30

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

//colasEspera[i] es la cola de mutex en la que el thread i ve si alguien
//pidió un merge
//vector< COLAS<mutex> > colasEspera


//el arbol resultado que solo modifica el thread que no le quedan
//nodos que agregar (o sea que es el último)
// arbolRta para devolver resultado en paralelo
// grafocompartido grafo original
Grafo* arbolRta;
Grafo* grafoCompartido;

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
	//OJOOOO TO DO, esto tiene que ser atómico!!!
	// Falta agregar el mutex obvio ajaj y preguntar si otro ya lo pinto 
	colores[nodo] = miTid;
	coloresArbol[miTid][nodo] = NEGRO;
	//Para no volver a elegirlo (desde el thread i)
	distanciaParal[miTid][nodo] = IMAX;
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

void *ThreadCicle(void* inThread){

	//OJO, este "input" existe uno para cada thread, o lo van pisando??
	inputThread input = *((inputThread *) inThread);

	//agarro el nodo ya elegido al azar que me pasaron
	int nodoActual = input.nodoInicialRandom;//input.nodoAlgo;
	int miTid = input.threadId;

	//Arbol propio
	Grafo* arbolMio = &(arbolesGenerados[miTid]);

	//ESTE CICLO ESTA COPIADO ASI NOMAS DEL SECUENCIAL, CHEQUEARLO
	for(int i = 0; i < grafoCompartido->numVertices; i++){
		//ACA HAY QUE AGREGAR QUE CHEQUEE SU COLA A VER SI HAY QUE MERGEAR
		arbolMio->numVertices += 1;

		//La primera vez no lo agrego porque necesito dos nodos para unir
		if(i > 0){
		  arbolMio->insertarEje(nodoActual,distanciaNodoParal[miTid][nodoActual],distanciaParal[miTid][nodoActual]);
		}
		//Lo pinto de NEGRO para marcar que lo agregué al árbol y borro la distancia
		// Esto va a tener que estar antes, con el cambio correspondiente en las distancias (que tiene que ser despues de agregarlo al arbol en caso de no mergeo)
		pintarNodoPararelo(nodoActual, miTid);
		//OJO, TO DO, este no lo modifique, hay que copiar el secuencial y modificaralo
		//Descubrir vecinos: los pinto y calculo distancias
		pintarVecinosParalelo(arbolMio,nodoActual, miTid);
		//Busco el nodo más cercano que no esté en el árbol, pero sea alcanzable
		nodoActual = min_element(distanciaParal[miTid].begin(),distanciaParal[miTid].end()) - distanciaParal[miTid].begin();
		
	}
	// Si llegue aca, es el resultado (unico thread que queda)
	arbolRta = arbolMio;
	return NULL;

}



// ?? Merge(?? (int threadid1(el que llamó), threadid2(el otro), algo más?)){

// }



void mstParalelo(Grafo *g, int cantThreads) {
	//Imprimo el grafo
	g->imprimirGrafo();

	//OJOO, TO DO, este grafo g habría que cambiarlo para que sea atómico no?
	//o podríamos hacer vector<mutex> de tamaño g->cantNodos y antes de pintar
	//un nodo pedimos permiso a su mutex, así podemos usar la struct grafo que dieron

	//Semilla random
	srand(time(0));

	// Arreglo para crear threads
	pthread_t thread[cantThreads];
	// Para que lo puedan ver todos los threads
	grafoCompartido = g;
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

		//OJO, hay que declarar el input de cada thread afuera del for
		//para que no muera y los threads se queden sin sus datos
		//(ahora está hecho así)
        pthread_create(&thread[tid], NULL, &ThreadCicle, &(inputs[tid]));

    }

	//ESPERAR A QUE LOS THREADS MUERAN
    for (int x = 0; x < cantThreads; ++x){
        pthread_join(thread[x], NULL);
    }
	//podríamos agregar que imprima el tiempo que tardó para medirlo,
	//o que lo guarde en algún archivo junto con el tamaño del grafo
	//y cosas así

	//OJO, hay que ver donde guardamos el arbol resultado, porque ya no
	//está en un solo lugar como el secuencial, el último thread que queda
	//que lo sabe porque no le quedan nodos que agregar debería guardar
	//su arbol en un arbolRta compartido
	cout << endl << "== RESULTADO == " << endl;
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
