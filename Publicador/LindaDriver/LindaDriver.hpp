#include <iostream>
#include <chrono>
#include <thread>
#include "Tupla.hpp"
#include "Socket.hpp"

using namespace std;

const int MESSAGE_SIZE = 4001; //mensajes de no más 4000 caracteres

class LindaDriver {
    private:

    // Direccion y puertos servidores linda
    Socket* server[3];
    int socket_fd[3];

    // Obtener los servidores linda del registro de despliegue
    void obtenerServidores(const string dir, const int puerto, string& dir_1, string& dir_2,
                           string& dir_3, int& puerto_1, int& puerto_2, int& puerto_3);
                           
    // Iniciar la comunicacion con los servidores linda
    void iniciarComunicacion(const string dir_1, const string dir_2, const string dir_3,
                             const int puerto_1, const int puerto_2, const int puerto_3);

    public:

    // Constructor LindaDriver.
    // Comunicacion con el registro de despliegue para obtener servidores.
    // Establecer conexion con los tres servidores.
    // Inicializar otros parametros
    LindaDriver(const string dir, const int puerto);
    // Desctructor LindaDriver
    // Cerramos las conexiones con los servidores Linda
    ~LindaDriver();
    //PostNote
    void PN(const Tupla t);
    // Pre: "p" y "t" tienen el mismo tamaño
    // Post: RemoveNote, siendo "p" el patrón y "t" la tupla
    void RN(const Tupla p, Tupla& t);
    // Pre: "p1" y "t1" tienen el mismo tamaño
    //      "p2" y "t2" tienen el mismo tamaño
    // Post: RemoveNotes, siendo "p1" y "p2" los patrones y "t1" y "t2" las tuplas
    void RN_2(const Tupla p1, const Tupla p2, Tupla& t1, Tupla& t2);
    // Pre: "p" y "t" tienen el mismo tamaño
    // Post: ReadNote, siendo "p" el patrón y "t" la tupla
    void RdN(const Tupla p, Tupla& t);
    // Pre: "p1" y "t1" tienen el mismo tamaño
    //      "p2" y "t2" tienen el mismo tamaño
    // Post: ReadNotes, siendo "p1" y "p2" los patrones y "t1" y "t2" las tuplas
    void RdN_2(const Tupla p1, const Tupla p2, Tupla& t1, Tupla& t2);


    void pruebaConexion();
};
