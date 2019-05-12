#include "grafo.h"
#include <fstream>

int Grafo::inicializar(string nombreArchivo) {

  ifstream archivo;
  archivo.open(nombreArchivo.c_str());
  if(!archivo) {
    cerr << nombreArchivo << " no existe en el directorio" << endl;
    return 0;
  }

  //Primera línea es la cantidad de vértices
  archivo >> numVertices;
  //La segunda la cantidad de ejes
  int numEjesArchivo;
  archivo >> numEjesArchivo;

  listaDeAdyacencias = map<int,vector<Eje>>();

  int v1, v2, peso;
  //Crear los ejes entre ambos nodos
  for(int ejes = 0; ejes < numEjesArchivo; ejes++){

    if(archivo.eof()){
      cerr << "Faltan ejes en el archivo" << endl;
      return 0;
    }

    archivo >> v1 >> v2 >> peso;

    insertarEje(v1,v2,peso);
  }

  archivo.close();
  return 1;
}

vector<Eje>::iterator Grafo::vecinosBegin(int num){
  return listaDeAdyacencias[num].begin();
}

vector<Eje>::iterator Grafo::vecinosEnd(int num){
  return listaDeAdyacencias[num].end();
}

void Grafo::insertarEje(int nodoA, int nodoB, int peso){
    //Agrego eje de nodoA a nodoB
    Eje ejeA(nodoB,peso);
    listaDeAdyacencias[nodoA].push_back(ejeA);

    //Agrego eje de nodoB a nodoA
    Eje ejeB(nodoA,peso);
    listaDeAdyacencias[nodoB].push_back(ejeB);

    incrementarTotalEjes();
}

void Grafo::incrementarTotalEjes(){
  numEjes += 1;
}

double Grafo::pesoOriginal(Grafo* original, int i, int j){

  for (vector<Eje>::iterator ejes_iterator = original->listaDeAdyacencias.at(i).begin(); ejes_iterator != original->listaDeAdyacencias.at(i).end(); ++ejes_iterator){
  //recorre todos los vecinos de i
     if (ejes_iterator->nodoDestino == j) return ejes_iterator->peso;
  }
}

double Grafo::pesoTotal(Grafo *original){
  // Double por si falla algo y da con coma, pa ver 
  double peso = 0;

  for (map<int,vector<Eje>>::iterator nodo_ptr = listaDeAdyacencias.begin(); nodo_ptr != listaDeAdyacencias.end(); ++nodo_ptr){
    //recorre todos los nodos del arbol
    for (vector<Eje>::iterator it = vecinosBegin(nodo_ptr->first); it != vecinosEnd(nodo_ptr->first); ++it){  
      //recorre los ejes de cada nodo
      peso += pesoOriginal(original, nodo_ptr->first, it->nodoDestino);
    }
  }
  return peso/2;
}

void Grafo::imprimirGrafo() {
  cout << "Cantidad de nodos: " << numVertices << endl;
  cout << "Cantidad de ejes: " << numEjes << endl;

  for (int i = 0; i < numVertices; ++i) {
    cout << "\t" << i << ": - ";
    for(const Eje &Eje : listaDeAdyacencias[i]) {
      cout << "(" << Eje.nodoDestino << "," << Eje.peso << ") - ";
    }
    cout << endl;
  }
}
