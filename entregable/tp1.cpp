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
void pintarNodo(int nodo, int thread){
	//OJOOOO TO DO, esto tiene que ser atómico!!!

	colores[nodo] = thread;
	//Para no volver a elegirlo (desde el thread i)
	distanciaParal[thread][num] = IMAX;
}


void *ThreadCicle(void* inThread){

	inputThread input = *((inputThread *) inThread);

	//Arbol propio
	//TO DO, hay que ponerle un nombre que dependa de cada thread
	//o hacer un arreglo de arboles
	Grafo arbol;

	//agarro el nodo ya elegido al azar que me pasaron
	int nodoActual = input.nodoInicialRandom;//input.nodoAlgo;


}



?? Merge(?? (int threadid1(el que llamó), threadid2(el otro), algo más?)){

}



void mstParalelo(Grafo *g, int cantThreads) {
	//Imprimo el grafo
	g->imprimirGrafo();

	//OJOO, TO DO, este grafo g habría que cambiarlo para que sea atómico no?
	//o podríamos hacer vector<mutex> de tamaño g->cantNodos y antes de pintar
	//un nodo pedimos permiso a su mutex, así podemos usar la struct grafo que dieron

	//Semilla random
	srand(time(0));

	//el arbol resultado que solo modifica el thread que no le quedan
	//nodos que agregar (o sea que es el último)
	Grafo arbolRta;

	//Por ahora no colorié ninguno de los nodos
	colores.assign(g->numVertices,BLANCO);
	//Por ahora no calculé ninguna distancia
	//en distancia[i] tengo vector de todas las dists al arbol i
	distanciaParal.assign(cantThreads, vector<int>(g->numVertices,IMAX));
	distanciaNodoParal.assign(cantThreads, vector<int>(g->numVertices,-1));


	//estas son las structs que le pasamos a cada thread
	vector<inputThread> inputs;
	inputs.assign(cantThreads, ) //le hacemos un constructor por defecto a la struct?

	//LANZAR LOS THREADS
	for (int tid = 0; tid < cantThreads; ++tid){

		//le pasamos a cada thread su numero de hijo(no el posta del SO, el nuestro)
		input[tid].threadId = tid;
		input[tid].nodoInicialRandom = rand() % g->numVertices;

		//OJO, hay que declarar el input de cada thread afuera del for
		//para que no muera y los threads se queden sin sus datos
		//(ahora está hecho así)
        pthread_create(&thread[tid], NULL, &ThreadCicle, &(input[tid]));
    }

	//ESPERAR A QUE LOS THREADS MUERAN
    //si se declara el tid adentro del for entonces
    //no es el mismo tid que arriba no?? porque eso generaba problemas
    //en el ejemplo que dió rozen en clase del helloWorld.
    //Igual pregunto de curiosidad, podemos ponerle "x" al "tid" este
    for (int tid = 0; tid < cantThreads; ++tid){
        pthread_join(thread[tid], NULL);
    }


	//podríamos agregar que imprima el tiempo que tardó para medirlo,
	//o que lo guarde en algún archivo junto con el tamaño del grafo
	//y cosas así

	//OJO, hay que ver donde guardamos el arbol resultado, porque ya no
	//está en un solo lugar como el secuencial, el último thread que queda
	//que lo sabe porque no le quedan nodos que agregar debería guardar
	//su arbol en un arbolRta compartido
	cout << endl << "== RESULTADO == " << endl;
 	arbolRta.imprimirGrafo();
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
  int cantThreads = int(argv[2]);

  Grafo g;

  if(cantThreads==1){  	
	  if( g.inicializar(nombre) == 1){

		//Corro el algoirtmo secuencial de g
		mstSecuencial(&g);

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
