//*****************************************************************
// File:   Tupla.cpp
// Author: PSCD-Unizar
// Date:   noviembre 2020
// Coms:
//*****************************************************************

#include <vector>
#include <string>
#include <cassert>
#include <Tupla.hpp>

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

Tupla::Tupla(Tupla& t) {
    data = new vector<string>(t.size(), "");
    for(int i = 0; i < t.size(); i++){
    	data->at(i) = t.get(i);
    }
}

Tupla::~Tupla() {
    delete data;
}

int Tupla::size() {
    return data->size();
}

string Tupla::to_string() {
    string res = "[";
    for (int i=0; i<data->size()-1; i++) {
        res = res + data->at(i) + ",";
    }
    res = res + data->at(data->size()-1) + "]";
    return res;
}

string Tupla::get(int pos) {
    return data->at(pos);
}

void Tupla::set(int pos, string value) {
    data->at(pos) = value;
}

void Tupla::from_string(string s) {
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

bool Tupla::match(Tupla p) {
    //To be done
    bool hacenMatch = true;
    if(p.size() == this -> size()){
    	for(int i = 0; i < p.size() && hacenMatch; i++){
    		if(!esPatron(p.get(i))){
    			hacenMatch = p.get(i) == this -> get(i);
    		}
    	}
    	return hacenMatch;
    }
    else{
    	return false;
    }
}

//Función auxiliar añadida

bool Tupla::esPatron(string s){
	return s.length() == 2 && s[0] == '?' && s[1] >= 'A' && s[1] <= 'Z';
}
