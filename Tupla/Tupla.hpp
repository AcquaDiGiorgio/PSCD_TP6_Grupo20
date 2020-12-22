//*****************************************************************
// File:   Tupla.hpp
// Author: PSCD-Unizar && Jorge Rodrigo Pérez
// Date:   noviembre 2020
// Coms:   
//*****************************************************************

#ifndef TUPLA_HPP
#define TUPLA_HPP

#include <iostream>
#include <string>
#include <vector> //estudiar la clase "vector" de C++

using namespace std;


class Tupla {
public:
	//constructores -----------------------------
	//Pre:  1<=n<=6
	//Post: Crea una tupla con n componentes ""
	Tupla(int n);
	//las siguientes crean e inicializan una tupla a partir de los parámetros
	Tupla(string s1);
	Tupla(string s1, string s2);
	Tupla(string s1, string s2, string s3);
	Tupla(string s1, string s2, string s3, string s4);
	Tupla(string s1, string s2, string s3, string s4, string s5);
	Tupla(string s1, string s2, string s3, string s4, string s5, string s6);
	//Creo una tupla a partir de otra. Se trata de un duplicado
	Tupla(const Tupla& t);
	
	//destructor -----------------------------
	~Tupla();

	//operadores -----------------------------
	//Pre:
	//Post: devuelve el tamaño de la tupla
	int size()const;

	//Pre:
	//Post: devuelve la representación de la tupla como un string.
	//Com:  Ver ejemplos en mainLindaDriver.cpp
	string to_string()const;

	//Pre:  "s" tiene el formato adecuado para una tupla: "[val1,val2,..]"
	//      correspondiente al tamaño de "this"
	//Post: "data" tomará el valor adecuado a partir de "s"
	void from_string(string s);

	//Pre:  0<="pos"<this->size()
	//Post: devuelve el elemento en la posición "pos" de la tupla
	string get(int pos)const;

	//Pre:  0<="pos"<this->size()
	//Post: pone "value" en la posición "pos" de la tupla	
	void set(int pos, string value);

	//Pre:  "p" es un patrón
	//Post: ¿Se emparejan "this" y "p"?
	bool match(Tupla p)const;
	
	
private:
	//la tupla se almacenará como un vector de strings. Dado que antes
	//de su declaración no se conoce el número de componentes, declaramos
	//"data" como un puntero para el que se reservará memoria en el constructor,
    //momento en el que conocerá el tamaño. No olvidar liberar la memoria en 
    //el destructor
	vector<string> *data;
	bool esPatron(string s)const;
};
#endif
