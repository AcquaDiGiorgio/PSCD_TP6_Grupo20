#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <list>
#include <Tupla.hpp>

#define RdNcode 1
#define RNcode 2
#define RdN_2code 3
#define RN_2code 4
using namespace std;
const int NSizeTuplas=6;



class Linda{
    private:
    //Struct que se utiliza para que un proceso publique la tupla que esta buscando, y se le despierte cuando se suba una que haga match
    struct PuntoEspera{
        int opcode;
        Tupla buscando;//Tupla que está buscando el proceso
        Tupla buscando2;
        condition_variable* despertar;//variable para ser despertado
        Tupla encontrada;//Tupla en la que se le devolverá la tupla encontrada
        Tupla encontrada2;
    };

    //Tenemos tuplas de 6 tamaños distintos
    list<Tupla> tuplas[NSizeTuplas];//6 listas de tuplas
    condition_variable espera[NSizeTuplas];//Los procesos comparten mutex 
    list<PuntoEspera*> puntosDeEspera[NSizeTuplas];//Lugar en el que esperan los procesos a que se suba una tupla que quieren

    mutex mtx[NSizeTuplas];//y variables de sinconización por cada lista de tuplas
    
    int lecturasEnEspera=0;
    int removesEnEspera=0;
    int lecturasRealizadas=0;
    int removesRealizados=0;
    int escriturasRealizadas=0;
    
    
    bool matchMultiple(const Tupla& p1 ,const Tupla& p2,const Tupla& t1,const Tupla& t2)const;

    bool esVariable(const string s)const;

    bool tuplaEsperada(const Tupla& t);

    void esperarTupla(const int opcode,const Tupla& t, Tupla& retVal, unique_lock<mutex>& lck);

    void esperarTupla(const int opcode,const Tupla& t,const Tupla& t2, Tupla& retVal,Tupla& retVal2, unique_lock<mutex>& lck);

    bool exsistePareja(const Tupla& p, const Tupla& p2, const Tupla& t1,  std::list<Tupla>::iterator& t2);
    
    public:
    int NumeroDeTuplas()const;

    int LecturasEnEspera()const;

    int RemovesEnEspera()const;

    int LecturasRealizadas()const;

    int RemovesRealizados()const;

    int EscriturasRealizadas()const;

    void GeneralInfo(int& NumeroDeTuplas,int& LecturasEnEspera,int& RemovesEnEpera,
        int& LecturasRealizadas,int& RemovesRealizados,int& EscriturasRealizadas)const;

    Linda ();

    void PN(Tupla t);

    Tupla RN(Tupla t);

    Tupla RdN(Tupla t);

	list<Tupla> RNM(Tupla t1, Tupla t2);

	list<Tupla> RdNM(Tupla t1, Tupla t2);
};
