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
    
    int lecturasEnEspera=0;
    int removesEnEpera=0;
    int lecturasRealizadas=0;
    int removesRealizados=0;
    int escriturasRealizadas=0;
    
    
    bool matchMultiple(const Tupla& p1 ,const Tupla& p2,const Tupla& t1,const Tupla& t2)const;

    bool esVariable(const string s)const;



    
    public:
    int NumeroDeTuplas()const;

    int LecturasEnEspera()const;

    int RemovesEnEpera()const;

    int LecturasRealizadas()const;

    int RemovesRealizados()const;

    int EscriturasRealizadas()const;

    void GeneralInfo(int& NumeroDeTuplas,int& LecturasEnEspera,int& RemovesEnEpera,
        int& LecturasRealizadas,int& RemovesRealizados,int& EscriturasRealizadas)const;

    Linda ();

    void PN(Tupla t);

    Tupla RN(Tupla t);

    Tupla RdN(Tupla t);
};

