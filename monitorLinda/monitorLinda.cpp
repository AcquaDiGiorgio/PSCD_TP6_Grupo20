#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <list>
#include <map>
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
    removesEnEpera++;
    Tupla retVal(sz);
    bool encontrado=false;
    //Mientras no ha encontrado una tupla que encaje
    while (!encontrado){
        //Se recorre toda la lista de tuplas
        for (std::list<Tupla>::iterator it = tuplas[sz].begin();
            (!encontrado) && it != tuplas[sz].end(); ++it){

            if (it->match(t)){
                retVal.copyFrom(*it);
                encontrado=true;
                tuplas[sz].erase(it);
            }
        }
        //Si no ha encontrado una tupla que le satisfazca, se duerme
        if (!encontrado) {
            espera[sz].wait(lck);
        }
    }
    removesEnEpera--;
    removesRealizados++;
    return retVal;
}

Tupla Linda::RdN(Tupla t) {
    int sz=t.size();
    unique_lock<mutex> lck(mtx[sz]);
    lecturasEnEspera++;
    bool encontrado=false;
    Tupla retVal(sz);
    //Mientras no ha encontrado una tupla que encaje
    while (!encontrado){
        //Se recorre toda la lista de tuplas
        for (std::list<Tupla>::iterator it = tuplas[sz].begin();
            (!encontrado) && it != tuplas[sz].end(); ++it){

            if (it->match(t)){
                retVal.copyFrom(*it);
                encontrado=true;
            }
        }
        //Si no ha encontrado una tupla que le satisfazca, se duerme
        if (!encontrado) {
            espera[sz].wait(lck);
        }
    }
    lecturasEnEspera--;
    lecturasRealizadas++;
    return retVal;
}

//Compara que los patrones p1 y p2 encajen con las tuplas p1 y p2
//IMPORTANTE: p1 y p2 tienen ser los patrones (los que contienen variables ?X) no t1 o t2
//El algoritmo usado es similar al metodo match de la clase Tupla
bool Linda::matchMultiple(const Tupla& p1 ,const Tupla& p2,const Tupla& t1,const Tupla& t2)const{
    map<char,string> variables;//en este map se almacenan un string asociado al nombre de una variable
    //Todas las variables con el mismo nombre tienen que tener el mismo string asociado o no hace match
    for (int i=0; i<p1.size();i++){
        string valor=p1.get(i);
        if(esVariable(valor)){
            //si es la primera vez que aparece el nombre de una variable, la añadimos al map
            if(variables.count(valor[1])==0){
                variables[valor[1]]=t1.get(i);
            }else{
                //si ya a aparecido anteriormente, pero con otro string asociado, no hacen match
                if(variables[valor[1]]!=t1.get(i)){
                    return false;
                }
            }
        }else if(valor!=t1.get(i)) {//si no es una variable y los valores no coinciden
            return false;//no hacen match
        }
    }
    for (int i=0; i<p2.size();i++){
        string valor=p2.get(i);
        if(esVariable(valor)){
            if(variables.count(valor[1])==0){
                variables[valor[1]]=t2.get(i);
            }else{
                if(variables[valor[1]]!=t2.get(i)){
                    return false;
                }
            }
        }else if(valor!=t2.get(i)) {//si no es una variable y los valores no coinciden
            return false;//no hacen match
        }
    }
    return true;

}

int Linda::NumeroDeTuplas()const{
    int retVal=0;
    for (int i=0;i<NSizeTuplas;i++){
        retVal+=tuplas[i].size();
    }
    return retVal;
}


bool Linda::esVariable(const string s)const {
	return s.length() == 2 && s[0] == '?' && s[1] >= 'A' && s[1] <= 'Z';
}

int Linda::LecturasEnEspera()const{
    return lecturasEnEspera;
}

int Linda::RemovesEnEpera()const{
    return removesEnEpera;
}

int Linda::LecturasRealizadas()const{
    return lecturasRealizadas;
}

int Linda::RemovesRealizados()const{
    return removesRealizados;
}

int Linda::EscriturasRealizadas()const{
    return escriturasRealizadas;
}

void Linda::GeneralInfo(int& nTuplas,int& RdNEnEspera,int& RNenEpera,
    int& RdNrealizadas,int& RNrealizados,int& PNrealizadas)const{
        nTuplas=NumeroDeTuplas();
        RdNEnEspera=lecturasEnEspera;
        RNenEpera=removesEnEpera;
        RdNrealizadas=lecturasRealizadas;
        RNrealizados=removesRealizados;
        PNrealizadas=escriturasRealizadas;
    }