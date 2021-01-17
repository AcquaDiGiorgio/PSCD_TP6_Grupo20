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


void servCliente (Socket &soc, int client_fd, Linda &monitor,int& clientesConectados) {
    int length = 100;
    string buffer = "";

    int rcv_bytes = soc.Recv(client_fd,buffer,length);
    if (rcv_bytes == -1) {  
        cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        soc.Close(client_fd);
        clientesConectados--;
        exit(1);
    }
    if (buffer != "INICIO SESION") { 
        cerr << "Peticion incorrecta: recibido \"" + buffer +
            "\" cuando se esperaba \"INICIO SESION\"\n";
        soc.Close(client_fd);
        clientesConectados--;
        exit(1);
    }

    int send_bytes = soc.Send(client_fd, "SESION ACEPTADA");
    if(send_bytes == -1) { 
        cerr << "Error al enviar datos: " + string(strerror(errno)) + "\n";
        // Cerramos los sockets
        soc.Close(client_fd);
        clientesConectados--;
        exit(1);
    }

    bool seguir = true;
    while (seguir) {
        rcv_bytes = soc.Recv(client_fd,buffer,length);
        if (rcv_bytes == -1) {  
            cerr << "Error al recibir datos: " + string(strerror(errno)) + "\n";
            // Cerramos los sockets
            soc.Close(client_fd);
            clientesConectados--;
            exit(1);
        }
        if (buffer == "FIN SESION") {  // TODO: Cierre ordinario
            seguir = false;
            soc.Close(client_fd);
            clientesConectados--;
        }

        else {
            // Separamos el mensaje y realizamos la operacion
            char* aux = new char[buffer.length()+1];
            strcpy(aux, buffer.c_str());
			string envio, e1, e2;
            string tok = strtok(aux, ":");
            int size = atoi(strtok(NULL, ":"));
            Tupla entrada1(size);
            e1 = strtok(NULL, ":");
            entrada1.from_string(e1);
            if (tok == "PN") {
                monitor.PN(entrada1);
                envio = "POST NOTE CORRECTO";
            }
            else if (tok == "RN") {
                Tupla salida = monitor.RN(entrada1);
                envio = salida.to_string();
            }

            else if (tok == "RdN") {
                Tupla salida = monitor.RdN(entrada1);
                envio = salida.to_string();
            }
            else if (tok == "MON") {
                int nTuplas,RdNEnEspera,RNenEspera,RdNrealizadas,RNrealizados,PNrealizadas;
                monitor.GeneralInfo(nTuplas,RdNEnEspera,RNenEspera,RdNrealizadas,RNrealizados,PNrealizadas);
                envio= to_string(clientesConectados) + ':' + to_string(nTuplas) + ':'+ to_string(RdNEnEspera) +':'
                + to_string(RNenEspera) + ':' + to_string(RdNrealizadas)+ ':' +to_string(RNrealizados) + ':' + to_string(PNrealizadas) + ':';
            }
			else{
				Tupla entrada2(size);
				e2 = strtok(NULL, ":");
				entrada2.from_string(e2);
				if (tok == "RN_2") {
					list<Tupla> salida = monitor.RNM(entrada1,entrada2);
					envio = salida.front().to_string() + salida.back().to_string();				
				}

				else if (tok == "RdN_2") {
					list<Tupla> salida = monitor.RdNM(entrada1,entrada2);
					envio = salida.front().to_string() + salida.back().to_string();
				}

            	else {  
                	cerr << "Peticion incorrecta: recibido \"" + buffer +
                	    "\" cuando se esperaba \"MON\", \"PN\", \"RN\", \"RdN\", \"RN2\" o \"RdN2\" \n";
                	soc.Close(client_fd);
					clientesConectados--;
                	exit(1);
            	}
			}	

            
            int send_bytes = soc.Send(client_fd, envio);
            if(send_bytes == -1) {  
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

    Linda monitor;

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
    int error_code = chan.Listen(1000);//Límite arbitrario de conexiones simultaneas
    if (error_code == -1) {  
        cerr << "Error en el listen: " + string(strerror(errno)) + "\n";
        // Cerramos el socket
        chan.Close(socket_fd);
        exit(1);
    }

    int conexiones=0;

    while (true) { 
        // Accept

        int client_fd = chan.Accept();

        if(client_fd == -1) { 
            cerr << "Error en el accept: " + string(strerror(errno)) + "\n";
            // Cerramos el socket
            chan.Close(socket_fd);
            exit(1);
        }
        conexiones++;
        thread cliente= thread(&servCliente, ref(chan), client_fd, ref(monitor),ref(conexiones));
        cliente.detach();
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