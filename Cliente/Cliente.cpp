//*****************************************************************
// File:   Cliente.cpp
// Author: Ruben Albacete Seren, (...)
// Date:   diciembre 2020
// Coms:   N clientes que realizan varias operacion PN, RN y RdN
//*****************************************************************
#include <iostream>
#include <chrono>
#include <string>
#include <thread>
#include <Tupla.hpp>
#include <LindaDriver.hpp>

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
void procesoCliente(string IP, int puerto) {

    srand(time(0));

    int option = -1;
    Tupla tupla(0);
    Tupla tupla2(0);
    Tupla tuplaLeer(0);
    Tupla tuplaLeer2(0);

    LindaDriver LD(IP, puerto);

    while( option != 0 ){
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
                cout << tuplaLeer.to_string() << endl;
                break;
            case 3:
                tupla.copyFrom(crearTupla());
                LD.RN(tupla, ref(tuplaLeer));
                cout << tuplaLeer.to_string() << endl;
                break;
            case 4:
                tupla.copyFrom(crearTupla());
                tupla2.copyFrom(crearTupla());
                LD.RdN_2(tupla, tupla2, ref(tuplaLeer), ref(tuplaLeer2));
                cout << tuplaLeer.to_string() << " - " << tuplaLeer2.to_string() << endl;
                break;
            case 5:
                tupla.copyFrom(crearTupla());
                tupla2.copyFrom(crearTupla());
                LD.RN_2(tupla, tupla2, ref(tuplaLeer), ref(tuplaLeer2));
                cout << tuplaLeer.to_string() << " - " << tuplaLeer2.to_string() << endl;
                break;
            case 0:
                cout << "Terminando..." << endl;
                break;
            default:
                cout << "Elija una opcion valida" << endl;
        }
    }   
}

int main(int argc, char *argv[]) {

    if( argc != 3 ){
        cout << "Datos de entrada mal introducidos" << endl;
        cout << "Ejecute como: Cliente *Direccion* *Puerto*" << endl;
        exit(1);
    }
    
    thread proceso = thread(&procesoCliente, argv[1], atoi(argv[2]));
    proceso.join();     
    return 0;
}
