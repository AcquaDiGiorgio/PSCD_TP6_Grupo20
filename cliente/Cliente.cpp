//*****************************************************************
// File:   Cliente.cpp
// Author: PSCD-Unizar
// Date:   noviembre 2015
// Coms:   Ejemplo de cliente con comunicación síncrona mediante sockets
//
//*****************************************************************
#include <iostream>
#include <chrono>
#include <thread>
#include <string.h>
#include "Socket.hpp"

using namespace std;

const int MESSAGE_SIZE = 4001; //mensajes de no más 4000 caracteres

int main() {
    srand(time(NULL));
    const string MENS_FIN = "END OF SERVICE";
    // Dirección y número donde escucha el proceso servidor
    string SERVER_ADDRESS = "localhost";
    int SERVER_PORT = 2000;

    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket chan(SERVER_ADDRESS, SERVER_PORT);

    // Conectamos con el servidor. Probamos varias conexiones
    const int MAX_ATTEMPS = 10;
    int count = 0;
    int socket_fd;
    do {
        // Conexión con el servidor
        socket_fd = chan.Connect();
        count++;

        // Si error --> esperamos 1 segundo para reconectar
        if(socket_fd == -1) {
            this_thread::sleep_for(chrono::seconds(1));
        }
    } while(socket_fd == -1 && count < MAX_ATTEMPS);

    // Chequeamos si se ha realizado la conexión
    if(socket_fd == -1) {
        return socket_fd;
    }

    string mensaje;
    int read_bytes;   //num de bytes recibidos en un mensaje
    int send_bytes;  //num de bytes enviados en un mensaje
    
    
    
    // Leer mensaje de la entrada estandar
        string solicitud="INICIO SESION";
   
        bool declinada=false;
        string buffer;
        // Enviamos el mensaje
        send_bytes = chan.Send(socket_fd, solicitud);
        read_bytes = chan.Recv(socket_fd, buffer, MESSAGE_SIZE);
        
        if(send_bytes == -1) {
            cerr << "Error al enviar datos 1: " << strerror(errno) << endl;
            // Cerramos el socket
            chan.Close(socket_fd);
            exit(1);
        }
        if(buffer=="SESION ACEPTADA"){
        	 int mensajes = 2;
        	do {
        		
        		//ENVIA PN o RN*****************************************************************
        		string reserva= "PN";
        		if(mensajes == 1){
        			reserva= "RN";
        		}
			cout<< "Operacion(" << mensajes << "): " << reserva << endl;
			send_bytes = chan.Send(socket_fd, reserva);
			if(send_bytes == -1) {
			    cerr << "Error al enviar datos: " << strerror(errno) << endl;
			    // Cerramos el socket
			    chan.Close(socket_fd);
			    exit(1);
			}
			//ENVIA PARAMETROS*****************************************************************
        		if(mensajes == 2){
        			reserva= "[ESTO,ES,UNA,PRUEBA]";
        		}
        		else{
        			reserva= "[?X,ES,UNA,PRUEBA]";
        		}
			cout << "Parametros enviados: " << reserva << endl;
			send_bytes = chan.Send(socket_fd, reserva);
			if(send_bytes == -1) {
			    cerr << "Error al enviar datos: " << strerror(errno) << endl;
			    // Cerramos el socket
			    chan.Close(socket_fd);
			    exit(1);
			}
			
			read_bytes = chan.Recv(socket_fd, mensaje, MESSAGE_SIZE);
			
			if(read_bytes == -1) {
			    cerr << "Error al recibir datos: " << strerror(errno) << endl;
			    // Cerramos el socket
			    chan.Close(socket_fd);
			    exit(1);
			}
			if(mensajes == 2){
				if(mensaje == "POST NOTE CORRECTO"){
					cout << "TODO GUAY(PN)" << endl << endl;
				}
			}
			else{
				if(mensaje == "[ESTO,ES,UNA,PRUEBA]"){
					cout << "TODO GUAY(RN)" << endl << endl;
				}
			}
			
			
			mensajes--;
			
			
	    	} while(mensaje != MENS_FIN && mensajes >0);
	    	
        	send_bytes = chan.Send(socket_fd, "FIN SESION");
        }
        
	
    

    // Cerramos el socket
    int error_code = chan.Close(socket_fd);
    if(error_code == -1) {
        cerr << "Error cerrando el socket: " << strerror(errno) << endl;
    }

    // Despedida
    cout << "Bye bye" << endl;

    return error_code;
}
