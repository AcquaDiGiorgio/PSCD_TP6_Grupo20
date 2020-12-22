#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <list>
#include "Tupla.hpp"
#include "monitorLinda.hpp"

using namespace std;





Linda::Linda (){
    for (int i=0;i<NSizeTuplas;i++){
        tuplas[i].empty();
    }
}

void Linda::PN(Tupla t) {//añade la tupla a su lista correspondiente
    int sz=t.size();
    unique_lock<mutex> lck(mtx[sz]);
    tuplas[sz].push_back(t);
    //Y avisa a todos procesos esperando en esa lista
    espera[sz].notify_all();
    escriturasRealizadas++;
}

Tupla Linda::RN(Tupla t) {
    int sz=t.size();
    unique_lock<mutex> lck(mtx[sz]);
    procesosEsperando++;
    Tupla retVal(sz);
    bool encontrado=false;
    //Mientras no ha encontrado una tupla que encaje
    while (!encontrado){
        //Se recorre toda la lista de tuplas
        for (std::list<Tupla>::iterator it = tuplas[sz].begin();
            (!encontrado) && it != tuplas[sz].end(); ++it){

            if (it->match(t)){
                retVal=*it;
                encontrado=true;
                tuplas[sz].erase(it);
            }
        }
        //Si no ha encontrado una tupla que le satisfazca, se duerme
        if (!encontrado) {
            espera[sz].wait(lck);
        }
    }
    procesosEsperando--;
    return retVal;
}

Tupla Linda::RdN(Tupla t) {
    int sz=t.size();
    unique_lock<mutex> lck(mtx[sz]);
    procesosEsperando++;
    bool encontrado=false;
    Tupla retVal(sz);
    //Mientras no ha encontrado una tupla que encaje
    while (!encontrado){
        //Se recorre toda la lista de tuplas
        for (std::list<Tupla>::iterator it = tuplas[sz].begin();
            (!encontrado) && it != tuplas[sz].end(); ++it){

            if (it->match(t)){
                retVal=*it;
                encontrado=true;
            }
        }
        //Si no ha encontrado una tupla que le satisfazca, se duerme
        if (!encontrado) {
            espera[sz].wait(lck);
        }
    }
    procesosEsperando--;
    return retVal;
}




