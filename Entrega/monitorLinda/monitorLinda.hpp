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
    //Struct que se utiliza para que un proceso publique la(s) tupla(s) que esta buscando, 
    //y se le despierte cuando se suba una que haga match y complete su operación
    struct PuntoEspera{
        int opcode;
        Tupla buscando;//Tuplas que está buscando el proceso
        Tupla buscando2;
        condition_variable* despertar;//variable condición para ser despertado
        Tupla encontrada;//Tuplas en la que se le devolverá la tupla encontrada
        Tupla encontrada2;
    };

    //Tenemos tuplas de 6 tamaños distintos
    list<Tupla> tuplas[NSizeTuplas];//6 listas de tuplas de diferentes tamaños
    list<PuntoEspera*> puntosDeEspera[NSizeTuplas];//Lugar en el que esperan los procesos a que se suba una tupla que quieren

    mutex mtx[NSizeTuplas];//Y mutex que comparten todas las operaciones que trabajan con tuplas del mimso tamaño
    
    int lecturasEnEspera=0;//Variables de monitorización del monitor
    int removesEnEspera=0;
    int lecturasRealizadas=0;
    int removesRealizados=0;
    int escriturasRealizadas=0;
    
    //OPERACIONES AUXILIARES PRIVADAS
    
    //Devuelve true si los patrones p1 y p2 hagan match con las tuplas t1 y t2
    //Devuelve false en caso contrario
    //IMPORTANTE: p1 y p2 tienen ser los patrones (los que contienen variables ?X) no t1 o t2
    //El algoritmo usado es similar al metodo match de la clase Tupla
    bool matchMultiple(const Tupla& p1 ,const Tupla& p2,const Tupla& t1,const Tupla& t2)const;

    //devuelve true si s tiene formato de variable
    //devuelve false en caso contrario
    bool esVariable(const string s)const;


    bool tuplaEsperada(const Tupla& t);

    //función auxiliar para RdN y RN. Duerme al proceso hasta que se publique una tupla que haga match con t, la cual se devuelve en t2 
    void esperarTupla(const int opcode,const Tupla& t, Tupla& retVal, unique_lock<mutex>& lck);

    //función auxiliar para RdN_2 y RN_2. Duerme al proceso hasta que hayan dos tuplas que hagan match con t y t2 
    //las cuales se devuelven en retVal y retVal2 
    void esperarTupla(const int opcode,const Tupla& t,const Tupla& t2, Tupla& retVal,Tupla& retVal2, unique_lock<mutex>& lck);

    //Dados los patrones p y p2 y la tupla t1 
    //si hay alguna tupla "t2" en el espacio de tuplas tal que matchMultiple(p,p2,t1,t2) devuelve true y devuelve el iterador de la tupla t2
    //en caso contrario devuelve false
    bool exsistePareja(const Tupla& p, const Tupla& p2, const Tupla& t1,  std::list<Tupla>::iterator& t2);
    
    public:

    //devuelve el número de tuplas que hay en el espacio de tuplas de este monitor
    int NumeroDeTuplas()const;

    //devuelve el número de lecturas en espera en este monitor
    int LecturasEnEspera()const;

    //devuelve el número de eliminaciones en espera en este monitor
    int RemovesEnEspera()const;

    //devuelve el número de lecturas realizadas en este monitor
    int LecturasRealizadas()const;

    //devuelve el número de eliminaciones realizadas en este monitor
    int RemovesRealizados()const;

    //devuelve el número de escrituras realizadas en este monitor
    int EscriturasRealizadas()const;

    //Devuelve información sobre el estado del monitor
    void GeneralInfo(int& NumeroDeTuplas,int& LecturasEnEspera,int& RemovesEnEpera,
        int& LecturasRealizadas,int& RemovesRealizados,int& EscriturasRealizadas)const;

    Linda ();

    //Añade una tupla al espacio de tuplas
    void PN(Tupla t);

    //Devuelve una tupla que haga match con t y la elimina del espacio de tuplas
    Tupla RN(Tupla t);

    //Devuelve una tupla que haga match con t
    Tupla RdN(Tupla t);

    //Devuelve una lista con 2 tuplas que hagan match con t1 y t2
    //t1 y t2 tienen que ser del mismo tamaño
	list<Tupla> RNM(Tupla t1, Tupla t2);

    //Devuelve una lista con 2 tuplas que hagan match con t1 y t2 y elimina a estas del espacio de tuplas
    //t1 y t2 tienen que ser del mismo tamaño
	list<Tupla> RdNM(Tupla t1, Tupla t2);
};
