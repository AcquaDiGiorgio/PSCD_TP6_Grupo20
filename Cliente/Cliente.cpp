//*****************************************************************
// File:   Cliente.cpp
// Author: Ruben Albacete Seren, (...)
// Date:   diciembre 2020
// Coms:   N clientes que realizan varias operacion PN, RN y RdN
//*****************************************************************
#include <iostream>
#include <chrono>
#include <thread>
#include "Tupla.hpp"
#include "LindaDriver.hpp"

using namespace std;

Tupla crearTupla(){
    int tamanyo = 0;
    string attb;

    cout << "Introduza el tamanyo de la tupla: ";
    cin >> tamanyo;
    Tupla tupla(tamanyo);

    for( int i = 0; i < tamanyo; i++ ){
        cout << "Introduza el atributo numero " << i+1 << ": ";
        cin >> attb;
        tupla.set(i, attb);
    }

    return tupla;
}

// Hilo de ejecucion de un cliente que interactua con el servidor linda
void procesoCliente(const string address, const int port) {

    srand(time(0));

    LindaDriver LD(address, port);
    int option = 0;
    Tupla tupla(0);
    Tupla tuplaLeer(0);
    
    while( option != 3 ){
        cout << "PN: 1, RdN: 2, RN: 3, RdN_2: 4, RN_2: 5, Salir: 0" << endl;
        cout << "Opcion: ";
        cin >> option;
        
        switch(option){
            case 1:
                tupla.copyFrom(crearTupla());
                LD.PN(tupla);
                break;
            case 2:
                tupla.copyFrom(crearTupla());
                LD.RdN(tupla, ref(tuplaLeer));
                cout << tuplaLeer.to_string();
                break;
            case 3:
                tupla.copyFrom(crearTupla());
                LD.RN(tupla, ref(tuplaLeer));
                break;
            case 0:
                cout << "Terminando..." << endl;
                break;
            default:
                cout << "Elija una opcion valida" << endl;
                cout << "Opciones 4 y 5 No Implementadas" << endl;
        }
    }   
}

int main(int argc, char *argv[]) {

    if( argc != 3 ){
        cout << "Datos de entrada mal introducidos" << endl;
        cout << "Ejecute como: Cliente *Direccion* *Puerto*" << endl;
        exit(1);
    }

    thread proceso = thread(&procesoCliente, argv[1], stoi(argv[2]));
    proceso.join();     
    return 0;
}
