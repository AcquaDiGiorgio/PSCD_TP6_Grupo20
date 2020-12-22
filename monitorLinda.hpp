#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <list>
#include "Tupla.hpp"


using namespace std;
const int NSizeTuplas=6;

class Linda{
    private:
    //Tenemos tuplas de 6 tamaños distintos
    list<Tupla> tuplas[NSizeTuplas];//6 listas de tuplas
    condition_variable espera[NSizeTuplas];//Los procesos comparten mutex 
    mutex mtx[NSizeTuplas];//y variables de sinconización por cada lista de tuplas
    
    int procesosEsperando=0;
    int lecturasRealizadas=0;
    int escriturasRealizadas=0;

    public:

    Linda ();

    void PN(Tupla t);

    Tupla RN(Tupla t);

    Tupla RdN(Tupla t);
};

