#ifndef GRAFO_H_
#define GRAFO_H_

#include <iostream>
#include <vector>
#include <map>

using namespace std;

typedef struct Eje {
  int nodoDestino;
  int peso;

  Eje(int nodoDestino, int peso) {
    this->nodoDestino = nodoDestino;
    this->peso = peso;
  }
  Eje():nodoDestino(0), peso(0) {}
} Eje;

class Grafo {

public:
  int numVertices;
  int numEjes;

  map<int,vector<Eje>> listaDeAdyacencias;

  Grafo() {
    numVertices = 0;
    numEjes = 0;
  }

  int inicializar(string nombreArchivo);
  void imprimirGrafo();
  vector<Eje>::iterator vecinosBegin(int num);
  vector<Eje>::iterator vecinosEnd(int num);
  void insertarEje(int nodoA, int nodoB, int peso);
  double pesoTotal();
private:
  void incrementarTotalEjes();
};

#endif
