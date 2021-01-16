//*****************************************************************
// File:   serverDespliegue.cpp
// Author: Jaime Garcia
// Date:   diciembre 2020
// Coms:
//*****************************************************************

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <thread>
#include "Socket.hpp"

using namespace std;



void servCliente(Socket& chan,int client_fd, string direcciones){
	string buffer;
	bool sigue = true;
	cout << "Datos a enviar a cliente " <<  client_fd << ": " << direcciones << endl;
	cout << "Conexion establecida con cliente " <<  client_fd << endl;
	while(sigue){
		int rcv_bytes = chan.Recv(client_fd,buffer,1000);
		if (rcv_bytes == -1) {
		    cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
		    // Cerramos los sockets
		    chan.Close(client_fd);
		    exit(1);
		}
		if(buffer == "DAME IPs"){
			int send_bytes = chan.Send(client_fd, direcciones);
			if (send_bytes == -1) {
				cerr << "Error al enviar datos: " + string(strerror(errno)) + "\n";
				chan.Close(client_fd);
			 	sigue = false;
			}
			cout << "Datos enviados a cliente " <<  client_fd << ": " << direcciones << endl;
		}
		else{
			cout << "Conexion cerrada con cliente " <<  client_fd << endl;
			chan.Close(client_fd);
			sigue = false;
	   	}
   	}
}

int main (int argc, char* argv[]) {
	string direcciones;
	int SERVER_PORT;
	if(argc == 1){
		direcciones = "192.168.1.70:50000-192.168.1.70:50001-192.168.1.70:50002";
		cout << "Direcciones y puertos por defecto : IPs:.70, Ports: 50000-50002" << endl;
		SERVER_PORT = 70000;
	}
	else if(argc == 8){
		string dir1, port1, dir2, port2, dir3, port3;
		dir1 = argv[2];port1 = argv[3]; dir2 = argv[4]; port2 = argv[5]; dir3 = argv[6]; port3 = argv[7];
		direcciones = dir1 + ":" + port1 + "-" + dir2 + ":" + port2 + "-" + dir3 + ":" + port3;
		SERVER_PORT = atoi(argv[1]);
	}
    else{
        cerr << "Numero de parametros incorrecto\nUso: " << argv[0]
             << " @puerto @ip_serverLinda1 @puerto_serverLinda1"
             << " @ip_serverLinda2 @puerto_serverLinda2"
             << " @ip_serverLinda3 @puerto_serverLinda3\n";
             exit(0);
             
    }
    
    
    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket chan(SERVER_PORT);

    // Bind
    int socket_fd = chan.Bind();
    if (socket_fd == -1) {
        cerr << "Error en el bind: " + string(strerror(errno)) + "\n";
        exit(1);
    }

    // Listen
    int error_code = chan.Listen(N);
    if (error_code == -1) {
        cerr << "Error en el listen: " + string(strerror(errno)) + "\n";
        // Cerramos el socket
        chan.Close(socket_fd);
        exit(1);
    }
    
    while (true) {
        // Accept
        int client_fd = chan.Accept();
        cout << "Aceptado cliente" << endl;
        if (client_fd[i] == -1) {
            cerr << "Error en el accept: " + string(strerror(errno)) + "\n";
            // Cerramos el socket
            chan.Close(socket_fd);
            exit(1);
        }
        thread cliente = thread(&servCliente, ref(chan), client_fd, direcciones);
        cliente.detach();
    }

    // Cerramos el socket del servidor
    error_code = chan.Close(socket_fd);
    if (error_code == -1) {
        cerr << "Error cerrando el socket del servidor: " + string(strerror(errno)) + " aceptado" + "\n";
    }

    // Despedida
    cout << "Bye bye" << endl;
    return error_code;
}
