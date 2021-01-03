//*****************************************************************
// File:   Cliente.cpp
// Author: Ruben Albacete Seren, (...)
// Date:   diciembre 2020
// Coms:   N clientes que realizan varias operacion PN, RN y RdN
//*****************************************************************
#include <iostream>
#include <chrono>
#include <thread>
#include "Socket.hpp"
#include "LindaDriver.hpp"

using namespace std;



// Conectar con el servidor de despliegue para obtener los servidores
// que se encuentran disponibles para operar.
void realizarDespliegue(const string address, const int port, string &serv_1, string &serv_2, string &serv_3) {
    serv_1 = "localhost 20001";
    serv_2 = "localhost 20002";
    serv_3 = "localhost 20003";

    // TODO: FUTURA CONEXION CON REGISTRO DE DESPLIEGUE
    /*
    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket chan(address, port);

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
        exit(1);
    }

    string mensaje, buffer;
    int send_bytes, read_bytes;

    // Solicitar datos al registro
    mensaje = "SERVIDORES LINDA";
    send_bytes = chan.Send(socket_fd, mensaje);
    if(send_bytes == -1) {
        cerr << "Error al enviar datos: " << strerror(errno) << endl;
        // Cerramos el socket
        chan.Close(socket_fd);
        exit(1);
    }

    // Recibir respuesta del servidor
    read_bytes = chan.Recv(socket_fd, buffer, MESSAGE_SIZE);
    if(read_bytes == -1) {
        cerr << "Error al recibir datos: " << strerror(errno) << endl;
        // Cerramos el socket
        chan.Close(socket_fd);
        exit(1);
    }

    // Cerramos el socket
    int error_code = chan.Close(socket_fd);
    if(error_code == -1) {
        cerr << "Error cerrando el socket: " << strerror(errno) << endl;
    }

    // TODO: Extraer ADDRESS PORT del mensaje recibido

    */


}

// Realizar n veces la operacion PN
void solicitarPN(Socket chan,int socket_fd,  const int n) {
    string mensaje, buffer;
    int send_bytes, read_bytes;
    // Enviar mensajes del tipo "PN:size:[tupla]"
    for(int i = 0; i < n; ++i) {
        mensaje = "PN:4:[" + to_string(i)+ "," + to_string(i+1) +"," + to_string(i+2) + ","
                           + to_string(i+3) + "]";
        send_bytes = chan.Send(socket_fd, mensaje);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            chan.Close(socket_fd);
            exit(1);
        }
        // Recibir respuesta del servidor
        read_bytes = chan.Recv(socket_fd, buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            chan.Close(socket_fd);
            exit(1);
        }
        if(buffer != "POST NOTE CORRECTO") {
            cerr << "Respuesta del servidor incorrecta: recibido \"" + buffer +
            "\" cuando se esperaba \"POST NOTE CORRECTO\"\n";
            chan.Close(socket_fd);
            exit(1);
        }
    }
}

// Realizar n veces la operacion RdN
void solicitarRdN(Socket chan, int socket_fd, const int n) {
    string mensaje, buffer;
    int send_bytes, read_bytes;
    // Enviar mensajes del tipo "RdN:size:[tupla]"
    for(int i = 0; i < n; ++i) {
        mensaje = "RdN:4:[" + to_string(i) + ",?A,?B,?C]";
        send_bytes = chan.Send(socket_fd, mensaje);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            chan.Close(socket_fd);
            exit(1);
        }
        // Recibir respuesta del servidor
        read_bytes = chan.Recv(socket_fd, buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            chan.Close(socket_fd);
            exit(1);
        }
        cout << "Tupla leida: " << buffer << endl;
    }
}

// Realizar n veces la operacion RN
void solicitarRN(Socket chan, int socket_fd, const int n) {
    string mensaje, buffer;
    int send_bytes, read_bytes;
    // Enviar mensajes del tipo "RN:size:[tupla]"
    for(int i = 0; i < n; ++i) {
        mensaje = "RN:4:[" + to_string(i) + ",?A,?B,?C]";
        send_bytes = chan.Send(socket_fd, mensaje);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            chan.Close(socket_fd);
            exit(1);
        }
        // Recibir respuesta del servidor
        read_bytes = chan.Recv(socket_fd, buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            chan.Close(socket_fd);
            exit(1);
        }
        cout << "Tupla borrada: " << buffer << endl;
    }
}

// Hilo de ejecucion de un cliente que interactua con el servidor linda
void procesoCliente(const string address, const int port) {

    string servidor_1, servidor_2, servidor_3;
    // Obtener servidores linda del registro de despliegue
    realizarDespliegue(address,port,servidor_1,servidor_2,servidor_3);

    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket chan(address, port);

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
        exit(1);
    }
    string mensaje, buffer;
    int read_bytes;   //num de bytes recibidos en un mensaje
    int send_bytes;  //num de bytes enviados en un mensaje

    // Enviamos solicitud de inicio de sesion
    mensaje = "INICIO SESION";
    send_bytes = chan.Send(socket_fd, mensaje);
    if(send_bytes == -1) {
        cerr << "Error al enviar datos: " << strerror(errno) << endl;
        // Cerramos el socket
        chan.Close(socket_fd);
        exit(1);
    }
    // Recibir respuesta del servidor
    read_bytes = chan.Recv(socket_fd, buffer, MESSAGE_SIZE);
    if(read_bytes == -1) {
        cerr << "Error al recibir datos: " << strerror(errno) << endl;
        // Cerramos el socket
        chan.Close(socket_fd);
        exit(1);
    }
    if(buffer == "SESION ACEPTADA") {
        cout << "Sesion aceptada" << endl;
        // Hacer peticiones a los servidores
        solicitarPN(chan, socket_fd, 5);
        cout << "PN realizado" << endl;
        solicitarRdN(chan, socket_fd, 5);
        cout << "RdN realizado" << endl;
        solicitarRN(chan, socket_fd, 5);
        cout << "RN realizado" << endl;

        // Enviamos solicitud de alta
        mensaje = "FIN SESION";
        send_bytes = chan.Send(socket_fd, mensaje);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            chan.Close(socket_fd);
            exit(1);
        }
    }
    else {  // TODO: Gestion de errores
        cerr << "Respuesta del servidor incorrecta: recibido \"" + buffer +
            "\" cuando se esperaba \"SESION ACEPTADA\"\n";
        chan.Close(socket_fd);
        exit(1);
    }

    // Cerramos el socket
    int error_code = chan.Close(socket_fd);
    if(error_code == -1) {
        cerr << "Error cerrando el socket: " << strerror(errno) << endl;
    }

    // Despedida
    cout << "Bye bye" << endl;

}

int main(int argc, char *argv[]) {


    
    return 0;
}
