#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <list>
#include <map>
#include <Tupla.hpp>
#include <monitorLinda.hpp>

using namespace std;

//Constructor
Linda::Linda (){
    for (int i=0;i<NSizeTuplas;i++){
        tuplas[i].empty();
    }
}

//Dados los patrones p y p2 y la tupla t1 
//si hay alguna tupla "t2" en el espacio de tuplas tal que matchMultiple(p,p2,t1,t2) devuelve true y devuelve el iterador de la tupla t2
//en caso contrario devuelve false
bool Linda::exsistePareja(const Tupla& p, const Tupla& p2, const Tupla& t1,  std::list<Tupla>::iterator& t2){
    int sz=p.size();
     for (std::list<Tupla>::iterator it = tuplas[sz-1].begin();
        it != tuplas[sz-1].end(); ++it){
        if (matchMultiple(p,p2,t1,*it)){
            t2=it;
            return true;
        }
    }
    return false;
}

//Esta función revisa hay algun proceso que este esperando la tupla t para realizar su operación
//Itera sobre los puntos de espera, despertando a los procesos con RdNs cuyos patrones hagan match con t
//hasta que se encuentre que un proceso que vaya a usar esa tupla para completar su operación de RN, en cuyo caso
//lo despertará y dejara de iterar sobre la lista y no despertará a los posibles procesos RdN o RN que haya después de él.
//Devuelve true si hay algún proceso que haya cogido esa tupla para realizar una operación de RN,
//en cuyo caso la tupla no tiene que ser insertada en el espacio de tuplas porque la ha borrado dicho proceso
//Devuelve true en caso contrario
bool Linda::tuplaEsperada(const Tupla& t){
    int sz=t.size();
    //Recorre todos los puntos de espera que esperen tuplas del tamaño de t
    for (std::list<PuntoEspera*>::iterator it = puntosDeEspera[sz-1].begin();
        it != puntosDeEspera[sz-1].end(); ++it){
        int op=(*it)->opcode;
        if (op==RNcode || op==RdNcode && t.match((*it)->buscando)){
            //Si t hace match con el patrón que buscaba un proceso de operacion RN o RdN
            //Despertamos dicho proceso pasandole la tupla t
            (*it)->encontrada.copyFrom(t);
            (*it)->despertar->notify_one();
            it=puntosDeEspera[sz-1].erase(it);
            //Borramos el punto de espera, ese proceso ha dejado de esperar
            
            if(op==RNcode){
                //El RN se ha llevado la tupla y no hay que seguir iterando sobre los puntos de espera 
                return true;
            }
            
        } else if(op==RN_2code || op==RdN_2code){
            std::list<Tupla>::iterator pareja;
            //Hay que mirar que haga match con alguno de los 2 patrones
            if (t.match((*it)->buscando)){
                if(exsistePareja((*it)->buscando,(*it)->buscando2,t,pareja)){
                    //Si se encuentra un match multiple, se envian las 2 tuplas y se despierta al proceso
                    (*it)->encontrada.copyFrom(t);
                    (*it)->encontrada2.copyFrom(*pareja);
                    (*it)->despertar->notify_one();
                    it=puntosDeEspera[sz-1].erase(it);

                    if(op==RN_2code){
                        tuplas[sz-1].erase(pareja);
                        return true;
                    }
                }

            }else if(t.match((*it)->buscando2)){
                if(exsistePareja((*it)->buscando2,(*it)->buscando,t,pareja)){
                    (*it)->encontrada2.copyFrom(t);
                    (*it)->encontrada.copyFrom(*pareja);
                    (*it)->despertar->notify_one();
                    it=puntosDeEspera[sz-1].erase(it);

                    if(op==RN_2code){
                        tuplas[sz-1].erase(pareja);
                        return true;
                    }
                }

            }

        }

    }//end primer for de puntos de epera
    return false;
}

//función auxiliar para RdN y RN. Duerme al proceso hasta que se publique una tupla que haga match con t, la cual se devuelve en t2 
void Linda::esperarTupla(const int opcode,const Tupla& t, Tupla& t2, unique_lock<mutex>& lck){
    int sz=t.size();

    PuntoEspera* punto= new PuntoEspera{
        opcode,
        Tupla(t),
        Tupla(0),
        new condition_variable,
        Tupla(sz),
        Tupla(0)
    };
    //cout << "BUSCO TUPLA" << t.to_string()<<endl;
    puntosDeEspera[sz-1].push_back(punto);
    punto->despertar->wait(lck);
    t2.copyFrom(punto->encontrada);
    delete punto->despertar;
    delete punto;
    //cout<< "DESPIERTO "<< t2.to_string()<<endl;
}

//función auxiliar para RdN_2 y RN_2. Duerme al proceso hasta que hayan dos tuplas que hagan match con t y t2 
//las cuales se devuelven en retVal y retVal2 
void Linda::esperarTupla(const int opcode,const Tupla& t,const Tupla& t2, Tupla& retVal,Tupla& retVal2, unique_lock<mutex>& lck){
    int sz=t.size();
    PuntoEspera* punto= new PuntoEspera{
        opcode,
        Tupla(t),
        Tupla(t2),
        new condition_variable,
        Tupla(sz),
        Tupla(sz)
    };
    puntosDeEspera[sz-1].push_back(punto);
    punto->despertar->wait(lck);
    retVal.copyFrom(punto->encontrada);
    retVal2.copyFrom(punto->encontrada2);
    delete punto->despertar;
    delete punto;
}

//Añade una tupla al espacio de tuplas
void Linda::PN(Tupla t) {//añade la tupla a su lista correspondiente
    int sz=t.size();
    if (!tuplaEsperada(t)){
        tuplas[sz-1].push_back(t);//Inserta la tupla
    }
    escriturasRealizadas++;
}

//Devuelve una tupla que haga match con t y la elimina del espacio de tuplas
Tupla Linda::RN(Tupla t) {
    int sz=t.size();
    unique_lock<mutex> lck(mtx[sz-1]);
    removesEnEspera++;
    Tupla retVal(sz);
    bool encontrado=false;
    //Mientras no ha encontrado una tupla que encaje
    //Se recorre toda la lista de tuplas
    for (std::list<Tupla>::iterator it = tuplas[sz-1].begin();
        (!encontrado) && it != tuplas[sz-1].end(); ++it){

        if (it->match(t)){
            retVal.copyFrom(*it);
            encontrado=true;
            tuplas[sz-1].erase(it);
        }
    }
    //Si no ha encontrado una tupla que le satisfazca, se duerme esperando 
    //que se publique una tupla que haga match con t
    if (!encontrado) {
        esperarTupla(RNcode,t,retVal,lck);
    }
    
    removesEnEspera--;
    removesRealizados++;
    return retVal;
}

//Devuelve una tupla que haga match con t
Tupla Linda::RdN(Tupla t) {
    int sz=t.size();
    unique_lock<mutex> lck(mtx[sz-1]);
    lecturasEnEspera++;
    bool encontrado=false;
    Tupla retVal(sz);
    //Mientras no ha encontrado una tupla que encaje
    //Se recorre toda la lista de tuplas
    for (std::list<Tupla>::iterator it = tuplas[sz-1].begin();
        (!encontrado) && it != tuplas[sz-1].end(); ++it){

        if (it->match(t)){
            retVal.copyFrom(*it);
            encontrado=true;
        }
    }
     //Si no ha encontrado una tupla que le satisfazca, se duerme esperando 
    //que se publique una tupla que haga match con t
    if (!encontrado) {
        esperarTupla(RdNcode,t,retVal,lck);
    }
    lecturasEnEspera--;
    lecturasRealizadas++;
    return retVal;
}

//Devuelve una lista con 2 tuplas que hagan match con t1 y t2
//t1 y t2 tienen que ser del mismo tamaño
list<Tupla> Linda::RNM(Tupla t1, Tupla t2){
	int sz = t1.size();
	unique_lock<mutex> lck(mtx[sz-1]);
	removesEnEspera += 2;
	
	list<Tupla> retVal;
	retVal.empty();

	bool encontrados = false;
	
    std::list<Tupla>::iterator it1 = tuplas[sz-1].begin();	
    std::list<Tupla>::iterator it2 = tuplas[sz-1].begin();
    it2++;
    
    Tupla retVal1(sz);
    Tupla retVal2(sz);

    //Mientras no han encontrado dos tupla que hagan match
    //Se recorre toda la lista de tuplas
    while(!encontrados && it1 != tuplas[sz-1].end()){
        while(!encontrados && it2 != tuplas[sz-1].end()){
            if(matchMultiple(t1,t2,*it1,*it2)){
                retVal1.copyFrom(*it1);
                retVal2.copyFrom(*it2);
                retVal.push_back(retVal1);
                retVal.push_back(retVal2);
                encontrados = true;
                tuplas[sz-1].erase(it1);
                tuplas[sz-1].erase(it2);
            }
            else{
                //cout << "Not matched" << endl;
            }	
            it2++;
        }
        it1++;
        it2 = it1++;
    }
        
    //Duerme el proceso
    if(!encontrados){
        esperarTupla(RN_2code,t1,t2,retVal1,retVal2,lck);
        retVal.push_back(retVal1);
        retVal.push_back(retVal2);
    }
		

	removesEnEspera -= 2;
    removesRealizados += 2;
	return retVal;
}

//Devuelve una lista con 2 tuplas que hagan match con t1 y t2 y elimina a estas del espacio de tuplas
//t1 y t2 tienen que ser del mismo tamaño
list<Tupla> Linda::RdNM(Tupla t1, Tupla t2){
	int sz = t1.size();
	unique_lock<mutex> lck(mtx[sz-1]);
	lecturasEnEspera += 2;

	list<Tupla> retVal;
	retVal.empty();

	bool encontrados = false;
    std::list<Tupla>::iterator it1 = tuplas[sz-1].begin();	
    std::list<Tupla>::iterator it2 = tuplas[sz-1].begin();
    it2++;
    
    Tupla retVal1(sz);
    Tupla retVal2(sz);

    //Mientras no han encontrado dos tupla que hagan match
    //Se recorre toda la lista de tuplas
    while(!encontrados && it1 != tuplas[sz-1].end()){
        while(!encontrados && it2 != tuplas[sz-1].end()){
            if(matchMultiple(t1,t2,*it1,*it2)){
                retVal1.copyFrom(*it1);
                retVal2.copyFrom(*it2);
                retVal.push_back(retVal1);
                retVal.push_back(retVal2);
                encontrados = true;
            }
            it2++;
        }
        it1++;
        it2 = it1++;
    }
        
    //Duerme el proceso
    if(!encontrados){
        esperarTupla(RdN_2code,t1,t2,retVal1,retVal2,lck);
        retVal.push_back(retVal1);
        retVal.push_back(retVal2);
    }

	

	lecturasEnEspera -= 2;
    lecturasRealizadas += 2;

    //cout<<"RdN_2 "<< retVal.front().to_string()<<retVal.back().to_string()<<endl;
	return retVal;
}

//Devuelve true si los patrones p1 y p2 hagan match con las tuplas t1 y t2
//Devuelve false en caso contrario
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

//devuelve el número de tuplas que hay en el espacio de tuplas de este monitor
int Linda::NumeroDeTuplas()const{
    int retVal=0;
    for (int i=0;i<NSizeTuplas;i++){
        retVal+=tuplas[i].size();
    }
    return retVal;
}

//devuelve true si s tiene formato de variable
//devuelve false en caso contrario
bool Linda::esVariable(const string s)const {
	return s.length() == 2 && s[0] == '?' && s[1] >= 'A' && s[1] <= 'Z';
}

//devuelve el número de lecturas en espera en este monitor
int Linda::LecturasEnEspera()const{
    return lecturasEnEspera;
}
//devuelve el número de eliminaciones en espera en este monitor
int Linda::RemovesEnEspera()const{
    return removesEnEspera;
}
//devuelve el número de lecturas realizadas en este monitor
int Linda::LecturasRealizadas()const{
    return lecturasRealizadas;
}
//devuelve el número de eliminaciones realizadas en este monitor
int Linda::RemovesRealizados()const{
    return removesRealizados;
}
//devuelve el número de escrituras realizadas en este monitor
int Linda::EscriturasRealizadas()const{
    return escriturasRealizadas;
}

//Devuelve información sobre el estado del monitor
void Linda::GeneralInfo(int& nTuplas,int& RdNEnEspera,int& RNenEpera,
    int& RdNrealizadas,int& RNrealizados,int& PNrealizadas)const{
        nTuplas=NumeroDeTuplas();
        RdNEnEspera=lecturasEnEspera;
        RNenEpera=removesEnEspera;
        RdNrealizadas=lecturasRealizadas;
        RNrealizados=removesRealizados;
        PNrealizadas=escriturasRealizadas;
    }

