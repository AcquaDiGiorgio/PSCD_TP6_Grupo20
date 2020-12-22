
//*****************************************************************
// File:   PLACEHOLDER_MONITOR.cpp
// Author: Jorge Rodrigo Pérez...
// Date:   Diciembre de 2020
// Coms:   Monitor de prueba para el/los server Linda del TP6
//*****************************************************************


#include "PLACEHOLDER_MONITOR.hpp"


//-----------------------------------------------------
using namespace std; //mutex, condition_variable, etc.
PLACEHOLDER_MONITOR::PLACEHOLDER_MONITOR(int a, int b, int c, int d){
	recursos[0] = a;
	recursos[1] = b;
	recursos[2] = c;
	recursos[3] = d;
	
}
//------------------------- destructor
PLACEHOLDER_MONITOR::~PLACEHOLDER_MONITOR(){}
//------------------------- usuario

//Pre: Recibe 4 parámetros de entrada a,b,c y d que serán los recursos que va a necesitar
//Post: Envía mensaje "RECURSOS CONCEDIDOS" y resta los recursos que se estan usando
void PLACEHOLDER_MONITOR::cogeRecursos(int a, int b, int c, int d, Socket& soc, int client_fd){ //Recursos que pide el cliente 
	unique_lock<mutex> lck(mtx);
	
	while(a > recursos[0] || b > recursos[1] || c > recursos[2] || d > recursos[3]){
		liberados.wait(lck); //Esperamos mientras los recursos esten ocupados
	}
	soc.Send(client_fd, "RECURSOS CONCEDIDOS");
	
	recursos[0] = recursos[0] - a;
	recursos[1] = recursos[1] - b;
	recursos[2] = recursos[2] - c;
	recursos[3] = recursos[3] - d;
}
//Pre: Recibe 4 parámetros de entrada a,b,c y d que serán los recursos que se van a dejar de usar
//Post: Suma los recursos que se han dejado de usar y hace un signal para avisar a "cogeRecursos" de que ha dejado recursos
void PLACEHOLDER_MONITOR::dejaRecursos(int a, int b, int c, int d){ //Recursos que deja el cliente
	unique_lock<mutex> lck(mtx);
	
	recursos[0] = recursos[0] + a;
	recursos[1] = recursos[1] + b;
	recursos[2] = recursos[2] + c;
	recursos[3] = recursos[3] + d;
	liberados.notify_all(); //Notifica al proceso "cogeRecursos" de que ha liberado recursos.
}
//Pre: Recibe 4 parámetros de entrada a,b,c y d que serán los recursos que va a necesitar
//Post: Devuelve true si los recursos no estan ocupados, devuelve false si estan ocupados.
bool PLACEHOLDER_MONITOR::mirarRecursos(int a, int b, int c, int d, Socket& soc, int client_fd){
	unique_lock<mutex> lck(mtx);
	if(a > recursos[0] || b > recursos[1] || c > recursos[2] || d > recursos[3]){
		soc.Send(client_fd, "RECURSOS OCUPADOS");
		return false;
	}
	else{
		return true;
	}

}















