#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <list>
#include <map>
#include <Tupla.hpp>
#include <monitorLinda.hpp>

using namespace std;


Linda::Linda (){
    for (int i=0;i<NSizeTuplas;i++){
        tuplas[i].empty();
    }
}

bool Linda::exsistePareja(const Tupla& p, const Tupla& p2, const Tupla& t1,  std::list<Tupla>::iterator& t2){
    int sz=p.size();
     for (std::list<Tupla>::iterator it = tuplas[sz].begin();
        it != tuplas[sz].end(); ++it){
        if (matchMultiple(p,p2,t1,*it)){
            t2=it;
            return true;
        }
    }
    return false;
}

bool Linda::tuplaEsperada(const Tupla& t){
    int sz=t.size();
    for (std::list<PuntoEspera*>::iterator it = puntosDeEspera[sz].begin();
        it != puntosDeEspera[sz].end(); ++it){
        int op=(*it)->opcode;
        if (op==RNcode || op==RdNcode && t.match((*it)->buscando)){
            //cout << "MATCH "<<t.to_string()<<endl;
            (*it)->encontrada.copyFrom(t);
            (*it)->despertar->notify_one();
            it=puntosDeEspera[sz].erase(it);
            
            if(op==RNcode){
                return true;
            }
            
        } else if(op==RN_2code || op==RdN_2code){
            std::list<Tupla>::iterator pareja;
            if (t.match((*it)->buscando)){
                if(exsistePareja((*it)->buscando,(*it)->buscando2,t,pareja)){
                    //cout << "MATCH MULTIPLE"<<t.to_string()<<(*pareja).to_string()<<endl;
                    (*it)->encontrada.copyFrom(t);
                    (*it)->encontrada2.copyFrom(*pareja);
                    (*it)->despertar->notify_one();
                    it=puntosDeEspera[sz].erase(it);

                    if(op==RN_2code){
                        tuplas[sz].erase(pareja);
                        return true;
                    }
                }

            }else if(t.match((*it)->buscando2)){
                if(exsistePareja((*it)->buscando2,(*it)->buscando,t,pareja)){
                    //cout << "MATCH MULTIPLE"<<t.to_string()<<(*pareja).to_string()<<endl;
                    (*it)->encontrada2.copyFrom(t);
                    (*it)->encontrada.copyFrom(*pareja);
                    (*it)->despertar->notify_one();
                    it=puntosDeEspera[sz].erase(it);

                    if(op==RN_2code){
                        tuplas[sz].erase(pareja);
                        return true;
                    }
                }

            }

        }

    }
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
    puntosDeEspera[sz].push_back(punto);
    punto->despertar->wait(lck);
    t2.copyFrom(punto->encontrada);
    delete punto->despertar;
    delete punto;
    //cout<< "DESPIERTO "<< t2.to_string()<<endl;
}

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
    //cout << "BUSCO TUPLAS" << t.to_string()<< t2.to_string()<<endl;
    puntosDeEspera[sz].push_back(punto);
    punto->despertar->wait(lck);
    retVal.copyFrom(punto->encontrada);
    retVal2.copyFrom(punto->encontrada2);
    delete punto->despertar;
    delete punto;
    //cout<< "DESPIERTO "<<retVal.to_string()<< retVal2.to_string()<<endl;
}

void Linda::PN(Tupla t) {//añade la tupla a su lista correspondiente
    int sz=t.size();
    unique_lock<mutex> lck(mtx[sz]);
    if (!tuplaEsperada(t)){
        tuplas[sz].push_back(t);
        //Y avisa a todos procesos esperando en esa lista
        espera[sz].notify_all();
    }
    escriturasRealizadas++;
}

Tupla Linda::RN(Tupla t) {
    int sz=t.size();
    unique_lock<mutex> lck(mtx[sz]);
    removesEnEspera++;
    Tupla retVal(sz);
    bool encontrado=false;
    //Mientras no ha encontrado una tupla que encaje
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
        esperarTupla(RNcode,t,retVal,lck);
    }
    
    removesEnEspera--;
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
        esperarTupla(RdNcode,t,retVal,lck);
    }
    lecturasEnEspera--;
    lecturasRealizadas++;
    return retVal;
}

list<Tupla> Linda::RNM(Tupla t1, Tupla t2){
	int sz = t1.size();
	unique_lock<mutex> lck(mtx[sz]);
	removesEnEspera += 2;
	
	list<Tupla> retVal;
	retVal.empty();

	bool encontrados = false;
	
    std::list<Tupla>::iterator it1 = tuplas[sz].begin();	
    std::list<Tupla>::iterator it2 = tuplas[sz].begin();
    it2++;
    
    Tupla retVal1(sz);
    Tupla retVal2(sz);

    while(!encontrados && it1 != tuplas[sz].end()){
        while(!encontrados && it2 != tuplas[sz].end()){
            if(matchMultiple(t1,t2,*it1,*it2)){
                retVal1.copyFrom(*it1);
                retVal2.copyFrom(*it2);
                retVal.push_back(retVal1);
                retVal.push_back(retVal2);
                encontrados = true;
                tuplas[sz].erase(it1);
                tuplas[sz].erase(it2);
            }
            else{
                //cout << "Not matched" << endl;
            }	
            it2++;
        }
        it1++;
        it2 = it1++;
    }
        

    if(!encontrados){
        esperarTupla(RN_2code,t1,t2,retVal1,retVal2,lck);
        retVal.push_back(retVal1);
        retVal.push_back(retVal2);
    }
		

	removesEnEspera -= 2;
    removesRealizados += 2;
	return retVal;
}

list<Tupla> Linda::RdNM(Tupla t1, Tupla t2){
	int sz = t1.size();
	unique_lock<mutex> lck(mtx[sz]);
	lecturasEnEspera += 2;

	list<Tupla> retVal;
	retVal.empty();

	bool encontrados = false;
    std::list<Tupla>::iterator it1 = tuplas[sz].begin();	
    std::list<Tupla>::iterator it2 = tuplas[sz].begin();
    it2++;
    
    Tupla retVal1(sz);
    Tupla retVal2(sz);

    
    while(!encontrados && it1 != tuplas[sz].end()){
        while(!encontrados && it2 != tuplas[sz].end()){
            if(matchMultiple(t1,t2,*it1,*it2)){
                retVal1.copyFrom(*it1);
                retVal2.copyFrom(*it2);
                retVal.push_back(retVal1);
                retVal.push_back(retVal2);
                encontrados = true;
            }
            else{
                //cout << "Not matched" << endl;
            }	
            it2++;
        }
        it1++;
        it2 = it1++;
    }
        

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

int Linda::RemovesEnEspera()const{
    return removesEnEspera;
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
        RNenEpera=removesEnEspera;
        RdNrealizadas=lecturasRealizadas;
        RNrealizados=removesRealizados;
        PNrealizadas=escriturasRealizadas;
    }

/* FUNCION DE BACKUP tupla esperada solo para operaciones simples
bool Linda::tuplaEsperada(const Tupla& t){
    int sz=t.size();
    for (std::list<PuntoEspera*>::iterator it = puntosDeEspera[sz].begin();
        it != puntosDeEspera[sz].end(); ++it){
        int op=(*it)->opcode;
        
        if (t.match((*it)->buscando)){
                
            cout << "MATCH "<<t.to_string()<<endl;
            (*it)->encontrada.copyFrom(t);
            (*it)->despertar->notify_one();
            it=puntosDeEspera[sz].erase(it);
            
            if(op==RNcode){
                return true;
            }
        }

    }
    return false;
}*/