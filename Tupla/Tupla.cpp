//*****************************************************************
// File:   Tupla.cpp
// Author: PSCD-Unizar
// Date:   noviembre 2020
// Coms:
//*****************************************************************

#include <vector>
#include <string>
#include <cassert>
#include "Tupla.hpp"

Tupla::Tupla(int n) {
    data = new vector<string>(n,"");
}

Tupla::Tupla(string s1) {
    data = new vector<string>(1,s1);
}

Tupla::Tupla(string s1, string s2) {
    data = new vector<string>(2,"");
    data->at(0) = s1;
    data->at(1) = s2;
    //alternativamente se podía haber hecho como
    //data = new vector<string>{s1,s2};
}

Tupla::Tupla(string s1, string s2, string s3) {
    data = new vector<string>(3,"");
    data->at(0) = s1;
    data->at(1) = s2;
    data->at(2) = s3;
}

Tupla::Tupla(string s1, string s2, string s3, string s4) {
    data = new vector<string>(4,"");
    data->at(0) = s1;
    data->at(1) = s2;
    data->at(2) = s3;
    data->at(3) = s4;
}

Tupla::Tupla(string s1, string s2, string s3, string s4, string s5) {
    data = new vector<string>(5,"");
    data->at(0) = s1;
    data->at(1) = s2;
    data->at(2) = s3;
    data->at(3) = s4;
    data->at(4) = s5;
}

Tupla::Tupla(string s1, string s2, string s3, string s4, string s5, string s6) {
    data = new vector<string>(6,"");
    data->at(0) = s1;
    data->at(1) = s2;
    data->at(2) = s3;
    data->at(3) = s4;
    data->at(4) = s5;
    data->at(5) = s6;
}

Tupla::Tupla(const Tupla& t){
    data = new vector<string>(t.size(), "");
    for(int i = 0; i < t.size(); i++){
    	data->at(i) = t.get(i);
    }

}

Tupla::~Tupla() {
    delete data;
}

int Tupla::size()const{
    return data->size();
}

string Tupla::to_string() const{
    string res = "[";
    for (int i=0; i<data->size()-1; i++) {
        res = res + data->at(i) + ",";
    }
    res = res + data->at(data->size()-1) + "]";
    return res;
}

string Tupla::get(const int pos) const{
    return data->at(pos);
}

void Tupla::set(const int pos,const string value) {
    data->at(pos) = value;
}

void Tupla::from_string(const string s) {
    assert(s.length()>2 && s[0]=='[' && s[s.size()-1]==']');
    string valores = "";
    int posicion = 0;
    for(int i = 0; i < s.length(); i++){
    	if(s[i] != '['){
    		if(s[i] == ',' || s[i] == ']'){
    			this -> set(posicion, valores);
    			posicion++;
    			valores = "";
    		}
    		else{
    			valores += s[i];
    		}
    	}
    	
    }
}

/*
void Tupla::from_string(const string s) {
    assert(s.length()>2 && s[0]=='[' && s[s.size()-1]==']');
    data=new vector<string>;
    char* msg= new char[s.length()];
    s.pop_back();
    strcpy(msg,&(s.c_str()[1]));//msg es el sting sin los corchetes [ ]
    char * pch = strtok (msg,",");
    for (int i=0;i>=6||pch!=nullptr;i++){//dividimos el sting por comas
        data->push_back(pch);//y vamos agregando los valores
        pch = strtok (NULL, ",");
    }
    delete msg;

}*/


bool Tupla::match(const Tupla p) const{
    map<char,string> variables;//en este map se almacenan un string asociado al nombre de una variable
    //Todas las variables con el mismo nombre tienen que tener el mismo string asociado o no hace match
    for(int i=0;i<p.size();i++){
        string valor=p.get(i);
        if(esPatron(valor)){
            //si es la primera vez que aparece el nombre de una variable, la añadimos al map
            if(variables.count(valor[1])==0){
                variables[valor[1]]=data->at(i);
            }else{
                //si ya a aparecido anteriormente, pero con otro string asociado, no hacen match
                if(variables[valor[1]]!=data->at(i)){
                   
                    return false;
                }
            }
        }else if(valor!=data->at(i)) {//si no es una variable y los valores no coinciden
            return false;//no hacen match
        }
    }
    return true;
}


//Función auxiliar añadida

bool Tupla::esPatron(const string s)const {
	return s.length() == 2 && s[0] == '?' && s[1] >= 'A' && s[1] <= 'Z';
}

