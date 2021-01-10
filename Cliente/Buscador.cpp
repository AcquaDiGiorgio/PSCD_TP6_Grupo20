//*****************************************************************
// File:   Buscador.cpp
// Author: Ruben Albacete Seren, Jorge Lisa Laborda
// Date:   diciembre 2020
// Coms:   N clientes que realizan varias operacion PN, RN y RdN
//*****************************************************************
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include "Tupla.hpp"
#include "LindaDriver.hpp"

using namespace std;

string getCiudadRandom(){
	srand(time(NULL)); 
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
			retVal = "Teruel";
		break;
		case 6:
			retVal = "Soria";
		break;
		case 7:
			retVal = "Tarragona";
		break;
		case 8:
			retVal = "Cuenca";
		break;
		case 9:
			retVal = "Huesca";
		break;
	}
	return retVal;
}

// Hilo de ejecucion de un cliente que interactua con LindaDriver
void procesoBuscador(string IP, int puerto) {

    LindaDriver LD(IP, puerto);
    int id = 0; // Hay que modificarlo para que no empiece siempre por la tupla 0

    Tupla tuplaLeer1(4), tuplaLeer2(4), tuplaLeer3(4);
    Tupla acceso("Acceso"), accesoLeer(1);

    for(int i = 0; i < 10; i++){

        LD.RN(acceso, accesoLeer);

    	Tupla tupla1(to_string(id), "?A", "?B", "?C");
    	Tupla tupla2(to_string(++id), "?A", "?B", "?C");
    	Tupla tupla3(to_string(++id), "?A", "?B", "?C");

    	LD.RdN(tupla1, ref(tuplaLeer1));
    	LD.RdN(tupla2, ref(tuplaLeer2));
    	LD.RdN(tupla3, ref(tuplaLeer3));

        cout << "Buscador Simple -> Tuplas leidas:\n" 
                + tuplaLeer1.to_string() + "\n"
                + tuplaLeer2.to_string() + "\n"
                + tuplaLeer3.to_string() + "\n";

        LD.PN(accesoLeer);

        this_thread::sleep_for(chrono::seconds(1));
    }

    cout << "Bye bye" << endl;
}

void procesoBuscadorCombinado(string IP, int puerto) {

    LindaDriver LD(IP, puerto);
    Tupla tuplaLeer1(4), tuplaLeer2(4);

    Tupla acceso("Acceso"), accesoLeer(1);

    for(int i = 0; i < 5; i++){

        LD.RN(acceso, accesoLeer);

    	string ciudad1 = getCiudadRandom();
    	string ciudad2 = getCiudadRandom();

    	while( ciudad1.compare(ciudad2) == 0 ) ciudad2 = getCiudadRandom();

    	Tupla tupla1("?A", getCiudadRandom(), "?X", "?B");
    	Tupla tupla2("?C", "?X", getCiudadRandom(), "?D");

    	LD.RdN_2(tupla1, tupla2, ref(tuplaLeer1), ref(tuplaLeer2));
        
        cout << "Buscador Combinado -> Tuplas leidas: " 
                + tuplaLeer1.to_string() + " - " + tuplaLeer2.to_string() + "\n";

        LD.PN(accesoLeer);
        this_thread::sleep_for(chrono::seconds(1));
    }

    cout << "Bye bye" << endl;
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        cout << "Datos de entrada mal introducidos" << endl;
        cout << "Ejecute como: Buscador *Direccion* *Puerto*" << endl;
        exit(1);
    }

    const int N_SIMPLES = 10;
    const int N_COMBINADOS = 5;

    thread simples[N_SIMPLES];
    thread combinados[N_COMBINADOS];

    for(int i = 0; i < N_SIMPLES; ++i) {
        simples[i] = thread(&procesoBuscador, argv[1], atoi(argv[2]));
    }
    for(int i = 0; i < N_COMBINADOS; ++i) {
        combinados[i] = thread(&procesoBuscadorCombinado, argv[1], atoi(argv[2]));
    }

    for(int j = 0; j < N_SIMPLES; ++j) {
        simples[j].join();
    }
    for(int i = 0; i < N_COMBINADOS; ++i) {
        combinados[i].join();
    }
    
    return 0;
}
