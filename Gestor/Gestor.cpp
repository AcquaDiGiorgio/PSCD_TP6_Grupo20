//*****************************************************************
// File:   Gestor.cpp
// Author: Jorge Rodrigo Perez,...
// Date:   Diciembre 2020
// Coms:   
//*****************************************************************


#include "Gestor.hpp"
using namespace std;

Gestor::Gestor(int tamanyo) {
    primeraTupla = nullptr;
    tamanyoTuplas = tamanyo;	//Tamaño de las tuplas a almacenar(no se usa, por ahora)
    numTuplas = 0;		//Numero de tuplas almacenadas en el Gestor
}
Gestor::~Gestor(){
	/*Nodo *actual, *siguiente;
	siguiente = actual;
	actual = primeraTupla;
	while(actual != nullptr){
		siguiente = actual -> siguiente;
		delete(actual);
		actual = siguiente;
	}*/
}
void Gestor::PN(Tupla t){
	if(primeraTupla == nullptr){//En caso de estar vacía
		primeraTupla = new Nodo;
		primeraTupla -> siguiente = nullptr;
		primeraTupla -> t = new Tupla(t);
	}
	else{				//En caso contrario se recorre la lista hasta encontrar el último elemento y se inserta al final
		Nodo* n = primeraTupla;
		while(n -> siguiente != nullptr){
			n = n -> siguiente;
		}
		n -> siguiente = new Nodo;
		n -> siguiente -> t = new Tupla(t);
		n ->siguiente -> siguiente = nullptr;
	}
	numTuplas++;
}
bool Gestor::RdN(Tupla& t){
	Nodo* n = primeraTupla;
	bool encontrado = false;
	
	while(!encontrado && n != nullptr){	//Se recorre la lista en busca de matches
		if(n -> t -> match(t)){	//Se comprueba si el elemento n->t hace match con t
			encontrado = true;
			t.from_string(n -> t -> to_string());	//Si se encuentra un match, se le asigna el valor de n->t a t y se sale del bucle
			
		}
		else{
			n = n -> siguiente;	//Si aun no se ha encontrado nada, se sigue buscando
		}
	}
	return encontrado;			//Devuelve true en caso de que se haya encontrado
}

bool Gestor::RN(Tupla& t){
	Nodo* n = primeraTupla;
	Nodo* anterior = n;
	bool encontrado = false;
	int i = 0;
	while(!encontrado && n != nullptr){
		if(n -> t -> match(t)){
			encontrado = true;
			
			t.from_string(n -> t -> to_string());
			if(i == 0){	//Si se encuentra en la primera posicion
				primeraTupla = n -> siguiente;
				delete(n);
			}
			else{
				anterior -> siguiente = n -> siguiente;
				delete(n);
			}
		}
		else{
			anterior = n;
			n = n -> siguiente;
			i++;
		}
	}
	return encontrado;	//Devuelve true en caso de que se haya encontrado
}

//Funcion para debugear el gestor. Simplemente imprime su contenido
void Gestor::print(){
	cout << endl << "----------------------------"  << endl;
	Nodo* p = primeraTupla;
	while(p != nullptr){
		cout << (p -> t -> to_string()) << endl;
		p = p -> siguiente;
	}
	cout << "----------------------------" << endl << endl;
}
