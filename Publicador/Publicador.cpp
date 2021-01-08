#include <thread>
#include <chrono>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <Tupla.hpp>
#include <LindaDriver.hpp>
#include <Socket.hpp>

using namespace std;

Tupla crearTupla(){
    int tamanyo = 0;
    string attb;

    cout << "Introduza el tamanyo de la tupla: ";
    cin >> tamanyo;
    Tupla tupla(tamanyo);

    for( int i = 0; i < tamanyo; i++ ){
        cout << "Introduza el atributo numero " << i+1 << ": ";
        cin >> attb;
        tupla.set(i, attb);
    }

    return tupla;
}

void generarOferta(string& origen, string& destino, int& precio, const string ciudades[]){
	srand(time(NULL));

	int nCiudad1 = rand() % (10-1);
	int nCiudad2 = rand() % (10-1);
	
	while(nCiudad1 == nCiudad2){
		nCiudad1 = rand() % (10-1);
		nCiudad2 = rand() % (10-1);
	}

	origen = ciudades[nCiudad1];
	destino = ciudades[nCiudad2];
	
	cout << "Inserte un precio (mínimo 0 euros): ";
	cin >> precio;
	while(precio < 0){
		cout << "El precio es incorrecto. Insértelo de nuevo: ";
		cin >> precio;
	}
}
 
void actualizarContador(LindaDriver& LD, int& id){
	Tupla contador(0);
	Tupla contadorLeer(0);

	contador.copyFrom(crearTupla());
	LD.RN(contador,ref(contadorLeer));
	id = stoi(contadorLeer.get(1));
	id++;
	contadorLeer.set(1,to_string(id));
	LD.PN(contadorLeer);
}

void publicarOfertas(LindaDriver& LD, const int nOfertas, const string ciudades[]){
	int precio,id;
	string origen, destino;
	
	for(int i = 0; i < nOfertas; i++){
		generarOferta(origen,destino,precio,ciudades);
		actualizarContador(LD,id);
		Tupla oferta(to_string(id),origen,destino,to_string(precio));
		LD.PN(oferta);
		cout << "Oferta número " << id << " publicada." << endl;
	}
}

void publicador(LindaDriver& LD, const string ciudades[]){

	int option, nOfertas;
	while(option != 0){
		cout << "Publicar oferta: 1, Salir: 0" << endl;
		cout << "Inserte una opcion: ";
		cin >> option;

		switch(option){	
			case 1: 
				cout << "Inserte el número de ofertas que desee publicar (mínimo 0): ";
				cin >> nOfertas;
				while(nOfertas < 0){
					cout << "Inserte un número válido, por favor: ";
					cin >> nOfertas;
				}
				publicarOfertas(LD,nOfertas,ciudades);
				break;

			case 0:
				cout << "Terminando..." << endl;
				break;

			default:
				cout << "Elija una opcion valida" << endl;
		}
		
	}
}

int main(int argc, char* argv[]){
	const int N = 5;
	const int N_CIUDADES = 10;
	thread p[N];

	string ciudades[N_CIUDADES] = {"Madrid", "Barcelona", "Andorra la Vella", "Zaragoza", "Valencia", "Teruel", "Soria", "Tarragona", "Cuenca","Huesca"};	

	LindaDriver LD(argv[1], stoi(argv[2]));	
	
	for(int i = 0; i < N; i++){
		p[i] = thread(&publicador,ref(LD),ref(ciudades));
	}

	for(int i = 0; i < N; i++){
		p[i].join();
	}

	return 0;
}
