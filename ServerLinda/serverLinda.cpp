//*****************************************************************
// File:   serverLinda.cpp
// Author: PSCD-Unizar
// Date:   diciembre 2020
// Coms:
//*****************************************************************

#include <iostream>
#include <thread>
#include <string>
#include <cstdlib>
#include <Socket.hpp>
#include <Tupla.hpp>
#include <monitorLinda.hpp>

using namespace std;

const int N = 5;  // TODO: Clientes simultaneos y/o totales usando el servidor

void servCliente (Socket &soc, int client_fd, Linda &monitor,int& clientesConectados) {
    int length = 100;
    string buffer = "";

    int rcv_bytes = soc.Recv(client_fd,buffer,length);
    if (rcv_bytes == -1) {  // TODO: Gestion de errores
        cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        soc.Close(client_fd);
        clientesConectados--;
        exit(1);
    }
    if (buffer != "INICIO SESION") {  // TODO: Gestion de errores
        cerr << "Peticion incorrecta: recibido \"" + buffer +
            "\" cuando se esperaba \"INICIO SESION\"\n";
        soc.Close(client_fd);
        clientesConectados--;
        exit(1);
    }

    int send_bytes = soc.Send(client_fd, "SESION ACEPTADA");
    if(send_bytes == -1) {  // TODO: Gestion de errores
        cerr << "Error al enviar datos: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        soc.Close(client_fd);
        clientesConectados--;
        exit(1);
    }

    bool seguir = true;
    while (seguir) {
        rcv_bytes = soc.Recv(client_fd,buffer,length);
        if (rcv_bytes == -1) {  // TODO: Gestion de errores
            cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
            // Cerramos los sockets
            soc.Close(client_fd);
            clientesConectados--;
            exit(1);
        }
        if (rcv_bytes == 0) {
            soc.Close(client_fd);
            return;
        }
        if (buffer == "FIN SESION") {  // TODO: Cierre ordinario
            seguir = false;
            soc.Close(client_fd);
            clientesConectados--;
        }

        else {
            // Separamos el mensaje y realizamos la operacion
            // TODO: Gestion de errores
            char* aux = new char[buffer.length()+1];
            strcpy(aux, buffer.c_str());
            string tok = strtok(aux, ":"), envio;
            int size = atoi(strtok(NULL, ":"));
            Tupla entrada(size);
            entrada.from_string(strtok(NULL, ":"));
            if (tok == "PN") {
                monitor.PN(entrada);
                envio = "POST NOTE CORRECTO";
            }

            else if (tok == "RN") {
                Tupla salida = monitor.RN(entrada);
                envio = salida.to_string();
            }

            else if (tok == "RdN") {
                Tupla salida = monitor.RdN(entrada);
                envio = salida.to_string();
            }
            else if (tok == "MON") {
                int nTuplas,RdNEnEspera,RNenEspera,RdNrealizadas,RNrealizados,PNrealizadas;
                monitor.GeneralInfo(nTuplas,RdNEnEspera,RNenEspera,RdNrealizadas,RNrealizados,PNrealizadas);
                envio= to_string(clientesConectados) + ':' + to_string(nTuplas) + ':'+ to_string(RdNEnEspera) +':'
                + to_string(RNenEspera) + ':' + to_string(RdNrealizadas)+ ':' +to_string(RNrealizados) + ':' + to_string(PNrealizadas) + ':';
                cout << envio;
            }

            else {  // TODO: Gestion de errores
                cerr << "Peticion incorrecta: recibido \"" + buffer +
                    "\" cuando se esperaba \"PN\", \"RN\" o \"RdN\"\n";
                soc.Close(client_fd);
                clientesConectados--;
                exit(1);
            }
            int send_bytes = soc.Send(client_fd, envio);
            if(send_bytes == -1) {  // TODO: Gestion de errores
                cerr << "Error al enviar datos: " + string(strerror(errno)) + "\n";
                // Cerramos los sockets
                soc.Close(client_fd);
                clientesConectados--;
                exit(1);
            }
        }
    }
}

int main (int argc, char *argv[]) {
    if (argc != 2)
    {
        printf("Número de parámetros incorrecto \n");
        printf("Uso: %s @puerto_srv\n", argv[0]);
        exit(1); // finaliza el programa indicando salida incorrecta (1)
    }

    int SERVER_PORT = atoi(argv[1]);
    thread cliente[N];
    int client_fd[N];
    Linda monitor;

    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket chan(SERVER_PORT);

    // Bind
    int socket_fd = chan.Bind();
    if (socket_fd == -1) {  // TODO: Gestion de errores
        cerr << "Error en el bind: " + string(strerror(errno)) + "\n";
        exit(1);
    }

    // Listen
    int error_code = chan.Listen(N);
    if (error_code == -1) {  // TODO: Gestion de errores
        cerr << "Error en el listen: " + string(strerror(errno)) + "\n";
        // Cerramos el socket
        chan.Close(socket_fd);
        exit(1);
    }

    for (int i = 0; i < N; i++) {  // TODO: Cientes a aceptar?
        // Accept
        int conexiones=0;

        client_fd[i] = chan.Accept();

        if(client_fd[i] == -1) {  // TODO: Gestion de errores
            cerr << "Error en el accept: " + string(strerror(errno)) + "\n";
            // Cerramos el socket
            chan.Close(socket_fd);
            exit(1);
        }
        conexiones++;
        cliente[i] = thread(&servCliente, ref(chan), client_fd[i], ref(monitor),ref(conexiones));
    }

    // ¿Qué pasa si algún thread acaba inesperadamente?
    for (int i=0; i<N; i++) {
        cliente[i].join();
    }

    // Cerramos el socket del servidor
    error_code = chan.Close(socket_fd);
    if (error_code == -1) {  // TODO: Gestion de errores
        cerr << "Error cerrando el socket del servidor: " + string(strerror(errno)) + " aceptado" + "\n";
    }

    // TODO: Fin del programa
    // Despedida
    cout << "Bye bye" << endl;
    return error_code;
}
