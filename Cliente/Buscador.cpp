//*****************************************************************
// File:   Buscador.cpp
// Author: Ruben Albacete Seren, Jorge Lisa Laborda
// Date:   diciembre 2020
// Coms:   N clientes que realizan varias operacion PN, RN y RdN
//*****************************************************************
#include <iostream>
#include <chrono>
#include <thread>
#include "Tupla.hpp"
#include "LindaDriver.hpp"

using namespace std;

string getCiudadRandom(){
	int value = rand() % 10;
	string retVal = "";
	switch(value){
		case 0:
			retVal = "Madrid";
		break;
		case 1:
			retVal = "Barcelona";
		break;
		case 2:
			retVal = "Valencia";
		break;
		case 3:
			retVal = "Oviedo";
		break;
		case 4:
			retVal = "Zaragoza";
		break;
		case 5:
			retVal = "Sevilla";
		break;
		case 6:
			retVal = "Salamanca";
		break;
		case 7:
			retVal = "Palma";
		break;
		case 8:
			retVal = "Murcia";
		break;
		case 9:
			retVal = "Bilbao";
		break;
	}
	return retVal;
}

// Hilo de ejecucion de un cliente que interactua con LindaDriver
void procesoBuscador(string IP, int puerto) {

    LindaDriver LD(IP, puerto);
    int id = 1; // Hay que modificarlo para que no empiece siempre por la tupla 1

    for(int i = 0; i < 10; i++){
    	Tupla tuplaLeer1(4);
	    Tupla tuplaLeer2(4);
	    Tupla tuplaLeer3(4);

    	Tupla tupla1(to_string(id), "?A", "?B", "?C");
    	Tupla tupla2(to_string(id++), "?A", "?B", "?C");
    	Tupla tupla3(to_string(id++), "?A", "?B", "?C");

    	LD.RdN(tupla1, ref(tuplaLeer1));
    	LD.RdN(tupla2, ref(tuplaLeer2));
    	LD.RdN(tupla3, ref(tuplaLeer3));
    }

    cout << "Bye bye" << endl;
}

void procesoBuscadorCombinado(string IP, int puerto) {

    LindaDriver LD(IP, puerto);
    Tupla tuplaLeer1(4);
    Tupla tuplaLeer2(4);

    for(int i = 0; i < 5; i++){
    	string ciudad1 = getCiudadRandom();
    	string ciudad2 = getCiudadRandom();

    	while( ciudad1 == ciudad2 ) ciudad2 = getCiudadRandom();

    	Tupla tupla1("?A", getCiudadRandom(), "?X", "?B");
    	Tupla tupla2("?C", "?X", getCiudadRandom(), "?D");

    	LD.RdN_2(tupla1, tupla2, ref(tuplaLeer1), ref(tuplaLeer2));
    }

    cout << "Bye bye" << endl;
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        cout << "Datos de entrada mal introducidos" << endl;
        cout << "Ejecute como: Cliente *Direccion* *Puerto*" << endl;
        exit(1);
    }

    const int N_CLIENTES = 2;
    thread cliente[N_CLIENTES];

    for(int i = 0; i < N_CLIENTES; ++i) {
        cliente[i] = thread(&procesoBuscador, argv[1], atoi(argv[2]));
    }

    for(int i = 0; i < N_CLIENTES; ++i) {
        cliente[i].join();
    }
    
    return 0;
}
