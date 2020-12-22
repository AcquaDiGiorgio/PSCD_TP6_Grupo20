
//*****************************************************************
// File:   PLACEHOLDER_MONITOR.hpp
// Author: Jorge Rodrigo Pérez...
// Date:   Diciembre de 2020
// Coms:   Fichero de interfaz del "PLACEHOLDER_MONITOR.cpp"
//*****************************************************************

#ifndef PLACEHOLDER_MONITOR_HPP
#define PLACEHOLDER_MONITOR_HPP
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <string>
#include <chrono>
#include <stdlib.h>
#include <thread>
#include "Socket.hpp"

//-----------------------------------------------------
using namespace std; //mutex, condition_variable, etc.
class PLACEHOLDER_MONITOR {
	private:
		int recursos[4]; //se le pasará el valor en el constructor
		mutex mtx; //para la ejecución de procs en exclusión mutua
		condition_variable liberados; //para esperar a que el tren esté vacı́o
		//...añadir las que se consideren necesarias
		
		//bool ofertaDeclinada = false;
		//...añadir las que se consideren necesarias
	public:
		//------------------------- constructores
		//el parámetro "N_W" se copiará sobre el atributo "N_W" del monitor
		PLACEHOLDER_MONITOR(int a, int b, int c, int d);
		//------------------------- destructor
		~PLACEHOLDER_MONITOR ();
		//------------------------- usuario
		
		void cogeRecursos(int a, int b, int c, int d, Socket& soc, int client_fd);
		void dejaRecursos(int a, int b, int c, int d);
		bool mirarRecursos(int a, int b, int c, int d, Socket& soc, int client_fd);
};
#endif
