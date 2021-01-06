#include "LindaDriver.hpp"

using namespace std;

// Obtener los servidores linda del registro de despliegue
void LindaDriver::obtenerServidores(const string dir, const int puerto, string& dir_1, string& dir_2,
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

// Iniciar la comunicacion con los servidores linda
void LindaDriver::iniciarComunicacion(const string dir_1, const string dir_2, const string dir_3,
                                      const int puerto_1, const int puerto_2, const int puerto_3)
{

    // Creación de los sockets con los que se llevará a cabo
    // la comunicación con los servidores
    server[0] = new Socket(dir_1,puerto_1);
    server[1] = new Socket(dir_2,puerto_2);
    server[2] = new Socket(dir_3,puerto_3);

    const int MAX_ATTEMPS = 10;
    int count;
    // Conectarse con cada servidor
    for(int i = 0; i < 3; ++i) {
        // Conectamos con el servidor. Probamos varias conexiones
        count = 0;
        do {
            // Conexión con el servidor
            socket_fd[i] = server[i]->Connect();
            count++;

            // Si error --> esperamos 1 segundo para reconectar
            if(socket_fd[i] == -1) {
                this_thread::sleep_for(chrono::seconds(1));
            }
        } while(socket_fd[i] == -1 && count < MAX_ATTEMPS);

        // Chequeamos si se ha realizado la conexión
        if(socket_fd[i] == -1) {
            exit(1);
        }
        string buffer;
        string mensaje = "INICIO SESION";
        int send_bytes = server[i]->Send(socket_fd[i], mensaje);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[i]->Close(socket_fd[i]);
            exit(1);
        }
        // Recibir respuesta del servidor
        int read_bytes = server[i]->Recv(socket_fd[i], buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[i]->Close(socket_fd[i]);
            exit(1);
        }
        if(buffer != "SESION ACEPTADA"){
             cerr << "Respuesta del servidor incorrecta: recibido \"" + buffer +
            "\" cuando se esperaba \"SESION ACEPTADA\"\n";
        server[i]->Close(socket_fd[i]);
        exit(1);
        }
    }
}

// Constructor LindaDriver.
// Comunicacion con el registro de despliegue para obtener servidores.
// Establecer conexion con los tres servidores.
// Inicializar otros parametros
LindaDriver::LindaDriver(const string dir, const int puerto){
    // Comunicacion con el registro de despliegue
    // Direccion y puertos servidores linda
    string dir_1, dir_2, dir_3;
    int puerto_1, puerto_2, puerto_3;
    obtenerServidores(dir,puerto,dir_1,dir_2,dir_3,puerto_1,puerto_2,puerto_3);
    // Establecer comunicacion con los servidores obtenidos
    iniciarComunicacion(dir_1,dir_2,dir_3,puerto_1,puerto_2,puerto_3);    
}

LindaDriver::~LindaDriver(){
    for(int i = 0; i < 3; i++){
        server[i]->Send(socket_fd[i], "FIN SESION");
        server[i]->Close(socket_fd[i]);
    }
}

void LindaDriver::pruebaConexion() {

    // Enviar mensaje de inicio a cada servidor a ver si responden
    string mensaje, buffer;
    int read_bytes;   //num de bytes recibidos en un mensaje
    int send_bytes;  //num de bytes enviados en un mensaje

    // Enviamos solicitud de inicio de sesion
    mensaje = "INICIO SESION";
    send_bytes = server[0]->Send(socket_fd[0], mensaje);
    send_bytes = server[1]->Send(socket_fd[1], mensaje);
    send_bytes = server[2]->Send(socket_fd[2], mensaje);

    // Recibir respuesta del servidor
    read_bytes = server[0]->Recv(socket_fd[0], buffer, MESSAGE_SIZE);
    cout << buffer << endl;
    read_bytes = server[1]->Recv(socket_fd[1], buffer, MESSAGE_SIZE);
    cout << buffer << endl;
    read_bytes = server[2]->Recv(socket_fd[2], buffer, MESSAGE_SIZE);
    cout << buffer << endl;
}

//PostNote
void LindaDriver::PN(const Tupla t) {
    // Convertir tupla a string para enviar
    string tupla = t.to_string(), buffer;
    int size = t.size();
    int send_bytes, read_bytes;
    int index;

    // Tupla valida
    if( ! ( size < 1 || size > 6 )){
        
        // Seleccion de servidor
        if(size >= 1 && size <= 3) index = 0;
        else if(size >= 4 && size <= 5) index = 1;
        else index = 2;

        string send = "PN:" + to_string(size) + ":" + tupla;

        send_bytes = server[index]->Send(socket_fd[index], send);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[index]->Close(socket_fd[index]);
            exit(1);
        }
        read_bytes = server[index]->Recv(socket_fd[index], buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[index]->Close(socket_fd[index]);
            exit(1);
        }
        if(buffer != "POST NOTE CORRECTO") {
            cerr << "Respuesta del servidor incorrecta: recibido \"" + buffer +
            "\" cuando se esperaba \"POST NOTE CORRECTO\"\n";
            server[index]->Close(socket_fd[index]);
            exit(1);
        }
        else {
            cout << buffer + ": " + tupla + "\n";
        }

    } else cout << "La tupla a enviar no es valida" << endl;
}

// Pre: "p" y "t" tienen el mismo tamaño
// Post: RemoveNote, siendo "p" el patrón y "t" la tupla
void LindaDriver::RN(const Tupla p, Tupla& t) {
    // Convertir tupla a string para enviar
    string patron = p.to_string(), buffer;
    int size = p.size();
    int send_bytes, read_bytes;
    int index;

    if( ! ( size < 1 || size > 6 )){
        
        if(size >= 1 && size <= 3) index = 0;
        else if(size >= 4 && size <= 5) index = 1;
        else index = 2;

        string send = "RN:" + to_string(size) + ":" + patron;

        send_bytes = server[index]->Send(socket_fd[index], send);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[index]->Close(socket_fd[index]);
            exit(1);
        }
        read_bytes = server[index]->Recv(socket_fd[index], buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[index]->Close(socket_fd[index]);
            exit(1);
        }
        // Convertir respuesta en tupla
        t.from_string(buffer);

    } else cout << "La tupla a enviar no es valida" << endl;
}

// Pre: "p1" y "t1" tienen el mismo tamaño
//      "p2" y "t2" tienen el mismo tamaño
// Post: RemoveNotes, siendo "p1" y "p2" los patrones y "t1" y "t2" las tuplas
void LindaDriver::RN_2(const Tupla p1, const Tupla p2, Tupla& t1, Tupla& t2) {
    // Convertir tupla a string para enviar
    string patron1 = p1.to_string(), patron2 = p2.to_string(), buffer;
    int size1 = p1.size(), size2 = p2.size();
    int send_bytes, read_bytes;
    int index;

    if( ! ( size1 < 1 || size1 > 6 || size1 != size2 ) ){
        int size = size1;
        string patron = patron1 + "," + patron2;
        //string send =  

        if(size >= 1 && size <= 3) index = 0;
        else if(size >= 4 && size <= 5) index = 1;
        else index = 2;

        send_bytes = server[index]->Send(socket_fd[index], "RN_2:" + patron);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[index]->Close(socket_fd[index]);
            exit(1);
        }
        read_bytes = server[index]->Recv(socket_fd[index], buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[index]->Close(socket_fd[index]);
            exit(1);
        }
        // Convertir respuesta en dos tuplas
        int pos = buffer.find("]") + 1;
        t1.from_string(buffer.substr(0,pos));
        t2.from_string(buffer.substr(pos+1,buffer.size()));

    }else if( size1 != size2 ) cout << "Las tuplas a enviar no tienen el mismo tamanyo" << endl;
    else cout << "El tamnyo de las tuplas a enviar no es valido" << endl;
}

// Pre: "p" y "t" tienen el mismo tamaño
// Post: ReadNote, siendo "p" el patrón y "t" la tupla
void LindaDriver::RdN(const Tupla p, Tupla& t) {
    // Convertir tupla a string para enviar
    string patron = p.to_string(), buffer;
    int size = p.size();
    int send_bytes, read_bytes;
    int index;

    if( ! ( size < 1 || size > 6 )){
        
        if(size >= 1 && size <= 3) index = 0;
        else if(size >= 4 && size <= 5) index = 1;
        else index = 2;
        string send = "RdN:" + to_string(size) + ":" + patron;

        send_bytes = server[index]->Send(socket_fd[index], send);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[index]->Close(socket_fd[index]);
            exit(1);
        }
        read_bytes = server[index]->Recv(socket_fd[index], buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[index]->Close(socket_fd[index]);
            exit(1);
        }
        // Convertir respuesta en tupla
        t.from_string(buffer);        

    }else cout << "La tupla a enviar no es valida" << endl;
}

// Pre: "p1" y "t1" tienen el mismo tamaño
//      "p2" y "t2" tienen el mismo tamaño
// Post: ReadNotes, siendo "p1" y "p2" los patrones y "t1" y "t2" las tuplas
void LindaDriver::RdN_2(const Tupla p1, const Tupla p2, Tupla& t1, Tupla& t2) {
    // Convertir tupla a string para enviar
    string patron1 = p1.to_string(), patron2 = p2.to_string(), buffer;
    int size1 = p1.size(), size2 = p2.size();
    int send_bytes, read_bytes;
    int index;
    
    if( ! ( size1 < 1 || size1 > 6 || size1 != size2) ){
        int size = size1;
        string patron = patron1 + "," + patron2;

        if(size >= 1 && size <= 3) index = 0;
        else if(size >= 4 && size <= 5) index = 1;
        else index = 2;
        //string send =  

        send_bytes = server[index]->Send(socket_fd[index], "RdN_2:" + patron);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[index]->Close(socket_fd[index]);
            exit(1);
        }
        read_bytes = server[index]->Recv(socket_fd[index], buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[index]->Close(socket_fd[index]);
            exit(1);
        }
        int pos = buffer.find("]") + 1;
        t1.from_string(buffer.substr(0,pos));
        t2.from_string(buffer.substr(pos+1,buffer.size()));

    }else if( size1 != size2 ) cout << "Las tuplas a enviar no tienen el mismo tamanyo" << endl;
    else cout << "El tamnyo de las tuplas a enviar no es valido" << endl;
}
