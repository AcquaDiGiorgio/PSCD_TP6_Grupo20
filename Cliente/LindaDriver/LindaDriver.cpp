#include "LindaDriver.hpp"

using namespace std;

// Obtener los servidores linda del registro de despliegue
void LindaDriver::obtenerServidores(const string dir, const int puerto, string& dir_1, string& dir_2,
                                    string& dir_3, int& puerto_1, int& puerto_2, int& puerto_3)
{

    dir_1 = "localdir"; dir_2 = "localdir"; dir_3 = "localdir";
    puerto_1 = 20000; puerto_2 = 20001; puerto_3 = 20002;
    // Creación del socket con el que se llevará a cabo
    // la comunicación con el servidor.
    /*Socket chan(dir, puerto);

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
    mensaje = "MENSAJE PETICION SERVIDORES";
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

*/
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
    }
}

// Constructor LindaDriver.
// Comunicacion con el registro de despliegue para obtener servidores.
// Establecer conexion con los tres servidores.
// Inicializar otros parametros
LindaDriver::LindaDriver (const string dir, const int puerto){
    // Comunicacion con el registro de despliegue
    // Direccion y puertos servidores linda
    string dir_1, dir_2, dir_3;
    int puerto_1, puerto_2, puerto_3;
    obtenerServidores(dir,puerto,dir_1,dir_2,dir_3,puerto_1,puerto_2,puerto_3);
    // Establecer comunicacion con los servidores obtenidos
    iniciarComunicacion(dir_1,dir_2,dir_3,puerto_1,puerto_2,puerto_3);
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
    if(size >= 1 && size <= 3) {
        // Enviar tupla de tamaño 1,2 o 3
        send_bytes = server[0]->Send(socket_fd[0], tupla);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[0]->Close(socket_fd[0]);
            exit(1);
        }
        read_bytes = server[0]->Recv(socket_fd[0], buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[0]->Close(socket_fd[0]);
            exit(1);
        }
        if(buffer != "POST NOTE CORRECTO") {
            cerr << "Respuesta del servidor incorrecta: recibido \"" + buffer +
            "\" cuando se esperaba \"POST NOTE CORRECTO\"\n";
            server[0]->Close(socket_fd[0]);
            exit(1);
        }
        else {
            cout << buffer + ": " + tupla + "\n";
        }
    }
    else if(size >= 4 && size <= 5) {
        // Enviar tupla de tamaño 4 o 5
        send_bytes = server[1]->Send(socket_fd[1], tupla);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[1]->Close(socket_fd[1]);
            exit(1);
        }
        read_bytes = server[1]->Recv(socket_fd[1], buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[1]->Close(socket_fd[1]);
            exit(1);
        }
        if(buffer != "POST NOTE CORRECTO") {
            cerr << "Respuesta del servidor incorrecta: recibido \"" + buffer +
            "\" cuando se esperaba \"POST NOTE CORRECTO\"\n";
            server[1]->Close(socket_fd[1]);
            exit(1);
        }
        else {
            cout << buffer + ": " + tupla + "\n";
        }
    }
    else if(size == 6) {
        // Enviar tupla de tamaño 6
        send_bytes = server[2]->Send(socket_fd[2], tupla);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[2]->Close(socket_fd[2]);
            exit(1);
        }
        read_bytes = server[2]->Recv(socket_fd[2], buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[2]->Close(socket_fd[2]);
            exit(1);
        }
        if(buffer != "POST NOTE CORRECTO") {
            cerr << "Respuesta del servidor incorrecta: recibido \"" + buffer +
            "\" cuando se esperaba \"POST NOTE CORRECTO\"\n";
            server[2]->Close(socket_fd[2]);
            exit(1);
        }
        else {
            cout << buffer + ": " + tupla + "\n";
        }
    }
    else {
        cout << "la tupla a enviar esta vacia\n";
    }
}

// Pre: "p" y "t" tienen el mismo tamaño
// Post: RemoveNote, siendo "p" el patrón y "t" la tupla
void LindaDriver::RN(const Tupla p, Tupla& t) {
    // Convertir tupla a string para enviar
    string patron = p.to_string(), buffer;
    int size = p.size();
    int send_bytes, read_bytes;
    if(size >= 1 && size <= 3) {
        // Enviar tupla de tamaño 1,2 o 3
        send_bytes = server[0]->Send(socket_fd[0], patron);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[0]->Close(socket_fd[0]);
            exit(1);
        }
        read_bytes = server[0]->Recv(socket_fd[0], buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[0]->Close(socket_fd[0]);
            exit(1);
        }
        // Convertir respuesta en tupla
        t.from_string(buffer);
    }
    else if(size >= 4 && size <= 5) {
        // Enviar tupla de tamaño 4 o 5
        send_bytes = server[1]->Send(socket_fd[1], patron);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[1]->Close(socket_fd[1]);
            exit(1);
        }
        read_bytes = server[1]->Recv(socket_fd[1], buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[1]->Close(socket_fd[1]);
            exit(1);
        }
        // Convertir respuesta en tupla
        t.from_string(buffer);
    }
    else if(size == 6) {
        // Enviar tupla de tamaño 6
        send_bytes = server[2]->Send(socket_fd[2], patron);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[2]->Close(socket_fd[2]);
            exit(1);
        }
        read_bytes = server[2]->Recv(socket_fd[2], buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[2]->Close(socket_fd[2]);
            exit(1);
        }
        // Convertir respuesta en tupla
        t.from_string(buffer);
    }
    else {
        cout << "la tupla a enviar esta vacia\n";
    }
}

// Pre: "p1" y "t1" tienen el mismo tamaño
//      "p2" y "t2" tienen el mismo tamaño
// Post: RemoveNotes, siendo "p1" y "p2" los patrones y "t1" y "t2" las tuplas
void LindaDriver::RN_2(const Tupla p1, const Tupla p2, Tupla& t1, Tupla& t2) {
    // Convertir tupla a string para enviar
    string patron1 = p1.to_string(), patron2 = p2.to_string(), buffer;
    int size1 = p1.size(), size2 = p2.size();
    int send_bytes, read_bytes;
    if(size1 > 0 && size1 == size2) {
        int size = size1;
        string patron = patron1 + "," + patron2;
        if(size >= 1 && size <= 3) {
            // Enviar patrones de tamaño 1,2 o 3
            send_bytes = server[0]->Send(socket_fd[0], patron);
            if(send_bytes == -1) {
                cerr << "Error al enviar datos: " << strerror(errno) << endl;
                // Cerramos el socket
                server[0]->Close(socket_fd[0]);
                exit(1);
            }
            read_bytes = server[0]->Recv(socket_fd[0], buffer, MESSAGE_SIZE);
            if(read_bytes == -1) {
                cerr << "Error al recibir datos: " << strerror(errno) << endl;
                // Cerramos el socket
                server[0]->Close(socket_fd[0]);
                exit(1);
            }
        }
        else if(size >= 4 && size <= 5) {
            // Enviar patrones de tamaño 4 o 5
            send_bytes = server[1]->Send(socket_fd[1], patron);
            if(send_bytes == -1) {
                cerr << "Error al enviar datos: " << strerror(errno) << endl;
                // Cerramos el socket
                server[1]->Close(socket_fd[1]);
                exit(1);
            }
            read_bytes = server[1]->Recv(socket_fd[1], buffer, MESSAGE_SIZE);
            if(read_bytes == -1) {
                cerr << "Error al recibir datos: " << strerror(errno) << endl;
                // Cerramos el socket
                server[1]->Close(socket_fd[1]);
                exit(1);
            }
        }
        else if(size == 6) {
            // Enviar patrones de tamaño 6
            send_bytes = server[2]->Send(socket_fd[2], patron);
            if(send_bytes == -1) {
                cerr << "Error al enviar datos: " << strerror(errno) << endl;
                // Cerramos el socket
                server[0]->Close(socket_fd[2]);
                exit(1);
            }
            read_bytes = server[2]->Recv(socket_fd[2], buffer, MESSAGE_SIZE);
            if(read_bytes == -1) {
                cerr << "Error al recibir datos: " << strerror(errno) << endl;
                // Cerramos el socket
                server[2]->Close(socket_fd[2]);
                exit(1);
            }
        }
        // Convertir respuesta en dos tuplas
        int pos = buffer.find("]") + 1;
        t1.from_string(buffer.substr(0,pos));
        t2.from_string(buffer.substr(pos+1,buffer.size()));
    }
    else {
        cout << "Los patrones a enviar estan vacios o bien size no coincice\n";
    }
}

// Pre: "p" y "t" tienen el mismo tamaño
// Post: ReadNote, siendo "p" el patrón y "t" la tupla
void LindaDriver::RDN(const Tupla p, Tupla& t) {
    // Convertir tupla a string para enviar
    string patron = p.to_string(), buffer;
    int size = p.size();
    int send_bytes, read_bytes;
    if(size >= 1 && size <= 3) {
        // Enviar tupla de tamaño 1,2 o 3
        send_bytes = server[0]->Send(socket_fd[0], patron);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[0]->Close(socket_fd[0]);
            exit(1);
        }
        read_bytes = server[0]->Recv(socket_fd[0], buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[0]->Close(socket_fd[0]);
            exit(1);
        }
        // Convertir respuesta en tupla
        t.from_string(buffer);
    }
    else if(size >= 4 && size <= 5) {
        // Enviar tupla de tamaño 4 o 5
        send_bytes = server[1]->Send(socket_fd[1], patron);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[1]->Close(socket_fd[1]);
            exit(1);
        }
        read_bytes = server[1]->Recv(socket_fd[1], buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[1]->Close(socket_fd[1]);
            exit(1);
        }
        // Convertir respuesta en tupla
        t.from_string(buffer);
    }
    else if(size == 6) {
        // Enviar tupla de tamaño 6
        send_bytes = server[2]->Send(socket_fd[2], patron);
        if(send_bytes == -1) {
            cerr << "Error al enviar datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[2]->Close(socket_fd[2]);
            exit(1);
        }
        read_bytes = server[2]->Recv(socket_fd[2], buffer, MESSAGE_SIZE);
        if(read_bytes == -1) {
            cerr << "Error al recibir datos: " << strerror(errno) << endl;
            // Cerramos el socket
            server[2]->Close(socket_fd[2]);
            exit(1);
        }
        // Convertir respuesta en tupla
        t.from_string(buffer);
    }
    else {
        cout << "la tupla a enviar esta vacia\n";
    }
}

// Pre: "p1" y "t1" tienen el mismo tamaño
//      "p2" y "t2" tienen el mismo tamaño
// Post: ReadNotes, siendo "p1" y "p2" los patrones y "t1" y "t2" las tuplas
void LindaDriver::RDN_2(const Tupla p1, const Tupla p2, Tupla& t1, Tupla& t2) {
    // Convertir tupla a string para enviar
    string patron1 = p1.to_string(), patron2 = p2.to_string(), buffer;
    int size1 = p1.size(), size2 = p2.size();
    int send_bytes, read_bytes;
    if(size1 > 0 && size1 == size2) {
        int size = size1;
        string patron = patron1 + "," + patron2;
        if(size >= 1 && size <= 3) {
            // Enviar patrones de tamaño 1,2 o 3
            send_bytes = server[0]->Send(socket_fd[0], patron);
            if(send_bytes == -1) {
                cerr << "Error al enviar datos: " << strerror(errno) << endl;
                // Cerramos el socket
                server[0]->Close(socket_fd[0]);
                exit(1);
            }
            read_bytes = server[0]->Recv(socket_fd[0], buffer, MESSAGE_SIZE);
            if(read_bytes == -1) {
                cerr << "Error al recibir datos: " << strerror(errno) << endl;
                // Cerramos el socket
                server[0]->Close(socket_fd[0]);
                exit(1);
            }
        }
        else if(size >= 4 && size <= 5) {
            // Enviar patrones de tamaño 4 o 5
            send_bytes = server[1]->Send(socket_fd[1], patron);
            if(send_bytes == -1) {
                cerr << "Error al enviar datos: " << strerror(errno) << endl;
                // Cerramos el socket
                server[1]->Close(socket_fd[1]);
                exit(1);
            }
            read_bytes = server[1]->Recv(socket_fd[1], buffer, MESSAGE_SIZE);
            if(read_bytes == -1) {
                cerr << "Error al recibir datos: " << strerror(errno) << endl;
                // Cerramos el socket
                server[1]->Close(socket_fd[1]);
                exit(1);
            }
        }
        else if(size == 6) {
            // Enviar patrones de tamaño 6
            send_bytes = server[2]->Send(socket_fd[2], patron);
            if(send_bytes == -1) {
                cerr << "Error al enviar datos: " << strerror(errno) << endl;
                // Cerramos el socket
                server[0]->Close(socket_fd[2]);
                exit(1);
            }
            read_bytes = server[2]->Recv(socket_fd[2], buffer, MESSAGE_SIZE);
            if(read_bytes == -1) {
                cerr << "Error al recibir datos: " << strerror(errno) << endl;
                // Cerramos el socket
                server[2]->Close(socket_fd[2]);
                exit(1);
            }
        }
        // Convertir respuesta en dos tuplas
        int pos = buffer.find("]") + 1;
        t1.from_string(buffer.substr(0,pos));
        t2.from_string(buffer.substr(pos+1,buffer.size()));
    }
    else {
        cout << "Los patrones a enviar estan vacios o bien size no coincice\n";
    }
}