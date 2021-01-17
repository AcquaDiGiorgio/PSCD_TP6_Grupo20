//*****************************************************************
// File:   clienteCarga.cpp
// Author: PSCD-Unizar
// Date:   enero 2021
// Coms:   TP6 curso 20-21. Cliente Linda de carga
//         invocar mediante 
//            clienteCarga <numTuplas> <IP_Registro> <Port_Registro> ...
//         Es un cliente sencillo para probar la respuesta del los
//         servidores linda. Genera un conjunto de patrones sencillos,
//         de tamaños desde 1 hasta 6, con todo variables distintas (de modo que cada 
//         patrón se puede emparejar con cualquier tupla de su tamaño).
//         Genera e inserta un conjunto de tuplas de cada uno de los
//         tamaños y, posteriormente, las descarga desde linda.
//         Se puede adapatar para ejecutar con distintos números
//         de tuplas, medir tiempos de ejecución, etc. 
//*****************************************************************
#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime> 

#include "LindaDriver.hpp"
#include "Tupla.hpp"

using namespace std;
//-----------------------------------------------------
void errorInvocacion() {
    cerr << "Invocar como:" << endl;
    cerr << "   clienteCarga <NT> <IP_LS> <Port_LS> ..." << endl;
    cerr << "      <NT>: num de tuplas" << endl;
    cerr << "      <IP_LS>: IP del servidor de registro" << endl;
    cerr << "      <Port_LS>: puerto del servidor de registro" << endl;
}
//-----------------------------------------------------
int main(int argc, char* argv[]) {
    time_t instanteInicial, instanteFinal;
    instanteInicial = time(0);
    const int MAX_LENGTH = 6; //máx longitud tuplas
    //faltan argumentos en la invocación?
    if (argc < 4 ) {
        errorInvocacion();
        return 1;
    }
    int NT = atoi(argv[1]); //num de tuplas a tratar
    // un driver con los datos pasados en la invocación
    LindaDriver LD(argv[2], atoi(argv[3]));
    // La conexión con los servidores Linda ya está establecida
    //-----------------------------------------------------
    //Creamos 6 tuplas, de tamaño 1 hasta 6, para ir rellenando y
    //Enviando a Linda sucesivamente. Se crean con cadenas vacías
    Tupla* tuplas[MAX_LENGTH];
    for (int j=0; j<MAX_LENGTH; j++) {
        tuplas[j] = new Tupla(j+1);
    }
    cout << "Tuplas creadas" << endl;
    //-----------------------------------------------------
    //Creamos 6 patrones, todos compuestos por solo variables,
    //por lo que cada patrón podrá emparejarse con cualquier tupla
    Tupla* patrones[MAX_LENGTH];
    patrones[0] = new Tupla("?A");
    patrones[1] = new Tupla("?A","?B");
    patrones[2] = new Tupla("?A","?B","?C");
    patrones[3] = new Tupla("?A","?B","?C","?D");
    patrones[4] = new Tupla("?A","?B","?C","?D","?E");
    patrones[5] = new Tupla("?A","?B","?C","?D","?E","?F");
    cout << "Patrones creados" << endl;
    //-----------------------------------------------------
    // Insertar 6*NT tuplas
    for (int i=1; i<=NT; i++) {
        string vT[MAX_LENGTH];
        vT[0] = to_string(i);
        for (int j=1; j<MAX_LENGTH; j++) {
            vT[j] = vT[j-1] + "," + to_string(i+j);
        }

        for (int j=0; j<MAX_LENGTH; j++) {
            tuplas[j]->from_string('[' + vT[j] + ']');
            // PosNote
            LD.PN(*tuplas[j]);
        }
    }
    cout << "Tuplas insertadas" << endl;
    //-----------------------------------------------------
    // Retirar 6*NT tuplas
    for (int i=1; i<=NT; i++) {
        for (int j=0; j<MAX_LENGTH; j++) {
            // RemoveNote
            LD.RN(*patrones[j],*tuplas[j]); //misma longitud
        }
        const int GRUPO = 50;
        // mostrar primera y última tuplas de cada grupo de GRUPO tuplas rescatadas
        if ((i-1)%GRUPO == 0) {
            cout << "    " << tuplas[0]->to_string() << endl;
            cout << "    " << tuplas[MAX_LENGTH-1]->to_string() << endl;
        }
    }
    cout << "Tuplas retiradas" << endl;
    //-----------------------------------------------------
    //liberar la memoria dinámica reservada
    for (int i=0; i<MAX_LENGTH; i++) {
        delete patrones[i];
        delete tuplas[i];
    }
    //-----------------------------------------------------
    instanteFinal = time(0);
    cout << "Execution time: " << instanteFinal - instanteInicial << " s" << endl;

    return 0;
}
