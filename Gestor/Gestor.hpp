//*****************************************************************
// File:   Gestor.hpp
// Author: Jorge Rodrigo Pérez, ...
// Date:   diciembre 2020
// Coms:   
//*****************************************************************

#ifndef GESTOR_HPP
#define GESTOR_HPP
#include <iostream>
#include <string>
#include "Tupla.hpp"
using namespace std;


/*Struct utilizado para implementar la memoria de forma dinámica */
struct Nodo{
	Nodo* siguiente = nullptr;//Puntero al siguiente Nodo de la lista enlazada
	Tupla* t = nullptr;	  //Puntero al valor(Tupla) del Nodo
};


/* La clase Gestor se encarga de introducir, leer y eliminar de la memoria tuplas de tamaño genérico (especificar en constructor)
 *  El Gestor implementa una lista enlazada de Nodos, cuyos valores son las tuplas
 TODO: FALTA IMPLEMENTAR RN_2 y RdN_2, además de la sincronización por monitores
 */
class Gestor {
public:
	Gestor(int tamanyo);
	~Gestor();
	void PN(Tupla t);
	bool RdN(Tupla& t);
	bool RN(Tupla& t);
	void print();

private:
	int tamanyoTuplas;
	int numTuplas;
	Nodo* primeraTupla;
};
#endif
