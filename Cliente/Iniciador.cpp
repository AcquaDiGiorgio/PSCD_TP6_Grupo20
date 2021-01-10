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

int main(int argc, char *argv[]) {

    if( argc != 3 ){
        cout << "Datos de entrada mal introducidos" << endl;
        cout << "Ejecute como: Cliente *Direccion* *Puerto*" << endl;
        exit(1);
    }
    
    LindaDriver LD(argv[1],atoi(argv[2]));
    Tupla contador("Contador","0");
    Tupla acceso("Acceso");

    LD.PN(contador);
    for(int i = 0; i < 8; i++){
        LD.PN(acceso);
    }     
    return 0;
}
