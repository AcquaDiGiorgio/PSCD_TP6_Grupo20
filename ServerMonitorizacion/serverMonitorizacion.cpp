#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>  
#include "Socket.hpp"

using namespace std;

const int MESSAGE_SIZE = 4001; //mensajes de no más 4000 caracteres
const int NLindaServers =3;
const int FrecuenciaMonitorizacion=5;

/*void realizarDespliegue(const string address, const int port, string ip[],int puerto[]) {
    //ip[0]= "localhost"; puerto[0]= 20001;
    //ip[1] = "localhost";puerto[1]= 20002;
    //ip[2] = "localhost";puerto[2]= 20003;

}*/

void obtenerServidores(const string dir, const int puerto, string& dir_1, string& dir_2,
                                    string& dir_3, int& puerto_1, int& puerto_2, int& puerto_3)
{

    /*dir_1 = "localdir"; dir_2 = "localdir"; dir_3 = "localdir";
    puerto_1 = 20000; puerto_2 = 20001; puerto_3 = 20002;*/
    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    Socket chan(dir, puerto);

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
    mensaje = "DAME IPs";
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

    // Extraer ADDRESS PORT del mensaje recibido
    // "IP1:P1-IP2:P2-IP3:P3"

    int pos = buffer.find("-");
    string dir_puerto = buffer.substr(0,pos);
    buffer = buffer.substr(pos+1);
    pos = dir_puerto.find(":");
    dir_1 = dir_puerto.substr(0,pos);
    puerto_1 = stoi(dir_puerto.substr(pos+1,dir_puerto.size()));

    pos = buffer.find("-");
    dir_puerto = buffer.substr(0,pos);
    buffer = buffer.substr(pos+1);
    pos = dir_puerto.find(":");
    dir_2 = dir_puerto.substr(0,pos);
    puerto_2 = stoi(dir_puerto.substr(pos+1,dir_puerto.size()));

    pos = buffer.find(":");
    dir_3 = buffer.substr(0,pos);
    puerto_3 = stoi(buffer.substr(pos+1,buffer.size()));


}

int main(int argc, char *argv[]) {
    if(argc < 3) {
        cout << "Invocar como: ./serverMonitorizacion IP PUERTO" << endl;
        exit(1);
    }
    // Dirección y puerto donde escucha el servidor
    string SERVER_ADDRESS = argv[1];
    int SERVER_PORT = stoi(argv[2]);

 
    // Obtener servidores linda del registro de despliegue
    string servidor[NLindaServers];//Hace falta un socket para cada servidor linda
    Socket chan[NLindaServers];
    int puerto[NLindaServers];
    //realizarDespliegue(SERVER_ADDRESS,SERVER_PORT,servidor,puerto);
    obtenerServidores(SERVER_ADDRESS, SERVER_PORT, ref(servidor[0]), ref(servidor[1]), ref(servidor[2]),
            ref(puerto[0]), ref(puerto[1]), ref(puerto[2]));
    
    //Inicializamos los Sockets
    for (int i=0;i<NLindaServers;i++){
        chan[i]=Socket(servidor[i], puerto[i]);
    }

    

    // Conectamos con los servidores. Probamos varias conexiones
    const int MAX_ATTEMPS = 10;

    int socket_fd[NLindaServers];
    
    for (int i=0;i<NLindaServers;i++){
        int count = 0;
        do {
            // Conexión con el servidor
            socket_fd[i] = chan[i].Connect();
            count++;

            // Si error --> esperamos 1 segundo para reconectar
            if(socket_fd[i] == -1) {
                this_thread::sleep_for(chrono::seconds(1));
            }
        } while(socket_fd[i] == -1 && count < MAX_ATTEMPS);
    }
    // Chequeamos si se ha realizado las conexiones correctamente
    for (int i=0;i<NLindaServers;i++){
        if(socket_fd[i] == -1) {
            cout << "No se ha podido establecer conexión con un servidor Linda" <<endl;
            exit(1);
        }
    }
    string mensaje, buffer;
    int read_bytes;   //num de bytes recibidos en un mensaje
    int send_bytes;  //num de bytes enviados en un mensaje

    // Enviamos solicitud de inicio de sesion a todos los servidores linda

    
    for (int i=0;i<NLindaServers;i++){
        mensaje = "INICIO SESION";
        send_bytes = chan[i].Send(socket_fd[i], mensaje);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            chan[i].Close(socket_fd[i]);
            exit(1);
        }
        // Recibir respuesta del servidor
        read_bytes = chan[i].Recv(socket_fd[i], buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            chan[i].Close(socket_fd[i]);
            exit(1);
        }
        if(buffer != "SESION ACEPTADA"){
             cerr << "Respuesta del servidor incorrecta: recibido \"" + buffer +
            "\" cuando se esperaba \"SESION ACEPTADA\"\n";
        chan[i].Close(socket_fd[i]);
        exit(1);
        }
    }
    
    bool fin=false;
    while (!fin){//TODO: Condición de salida
        //Parametros a medir
        int clientesConectados=0,nTuplas=0,RdNEnEspera=0,RNenEspera=0,RdNrealizadas=0,RNrealizados=0,PNrealizadas=0;
        
        //Enviamos las peticiones a todos los servidores y recopilamos los datos
        for (int i=0;i<NLindaServers;i++){
            send_bytes = chan[i].Send(socket_fd[i], "MON:1:[1]");//La petición tiene esta forma, porque el servidor linda tiene que leer una tupla
            if(send_bytes == -1) {
                cerr << "Error al enviar datos: " << strerror(errno) << endl;
                // Cerramos el socket
                chan[i].Close(socket_fd[i]);
                exit(1);
            }
            // Recibir respuesta del servidor
            read_bytes = chan[i].Recv(socket_fd[i], buffer, MESSAGE_SIZE);
            if(read_bytes == -1) {
                cerr << "Error al recibir datos: " << strerror(errno) << endl;
                // Cerramos el socket
                chan[i].Close(socket_fd[i]);
                exit(1);
            }

            char* aux = new char[buffer.length()+1];
            strcpy(aux, buffer.c_str());
            clientesConectados += atoi(strtok(aux, ":"));
            nTuplas += atoi(strtok(NULL, ":"));
            RdNEnEspera+=atoi(strtok(NULL, ":"));            
            RNenEspera+=atoi(strtok(NULL, ":"));
            RdNrealizadas+=atoi(strtok(NULL, ":"));
            RNrealizados+=atoi(strtok(NULL, ":"));
            PNrealizadas+=atoi(strtok(NULL, ":"));
            delete aux;
        }

        //Mostramos por pantalla las mediciones de los servidores
        time_t my_time = time(NULL); 
        cout << ctime(&my_time)<<endl;
        cout << "Clientes Conectados: " << clientesConectados << endl;
        cout << "Numero de tuplas en los servidores Linda: " << nTuplas << endl;
        cout << "Lecturas (RdN) en espera: " << RdNEnEspera << endl;
        cout << "Eliminaciones (RN) en espera: " << RNenEspera << endl;
        cout << "Lecturas realizadas: " << RdNrealizadas << endl;
        cout << "Eliminaciones realizadas: " << RNrealizados << endl;
        cout << "Escrituras realizadas: " << PNrealizadas << endl <<endl<<endl;
        //dormimos un rato...
        sleep(FrecuenciaMonitorizacion);

    }



    // Enviamos solicitud de alta a todos los servidores
    mensaje = "FIN SESION";
    for (int i=0;i<NLindaServers;i++){
        send_bytes = chan[i].Send(socket_fd[i], mensaje);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            chan[i].Close(socket_fd[i]);
            exit(1);
        }

        // Cerramos el socket
        int error_code = chan[i].Close(socket_fd[i]);
        if(error_code == -1) {
            cerr << "Error cerrando el socket: " << strerror(errno) << endl;
        }
    }
    // Despedida
    cout << "Bye bye" << endl;

}