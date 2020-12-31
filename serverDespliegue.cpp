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
#include "Socket.hpp"

using namespace std;

const int N = 5;  // TODO: Clientes en cola usando el servidor

int main (int argc, char* argv[]) {
    if (argc != 8) {
        cerr << "Numero de parametros incorrecto\nUso: " << argv[0]
             << " @puerto @ip_serverLinda1 @puerto_serverLinda1"
             << " @ip_serverLinda2 @puerto_serverLinda2"
             << " @ip_serverLinda3 @puerto_serverLinda3\n";
    }

    int SERVER_PORT = atoi(argv[1]);
    int client_fd;

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

    while (true) {  // TODO: Cientes a aceptar?
        // Accept
        client_fd = chan.Accept();

        if (client_fd == -1) {
            cerr << "Error en el accept: " + string(strerror(errno)) + "\n";
            // Cerramos el socket
            chan.Close(socket_fd);
            exit(1);
        }

        char msg[100];
        sprintf(msg, "%s:%s-%s:%s-%s:%s", argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
        int send_bytes = chan.Send(client_fd, msg);
        if (send_bytes == -1) {
            cerr << "Error al enviar datos: " + string(strerror(errno)) + "\n";
        }
        // Cerramos los sockets
        chan.Close(client_fd);
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
