#include <thread>
#include <chrono>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <Tupla.hpp>
#include <LindaDriver.hpp>
#include <Socket.hpp>

using namespace std;

string getCiudadRandom(){
	srand(time(NULL)); 
    int value = rand() % 10;
	string retVal = "";
	switch(value){
		case 0:
			retVal = "Madrid";
		break;
		case 1:
			retVal = "Barcelona";
		break;
		case 2:
			retVal = "Valencia";
		break;
		case 3:
			retVal = "Oviedo";
		break;
		case 4:
			retVal = "Zaragoza";
		break;
		case 5:
			retVal = "Teruel";
		break;
		case 6:
			retVal = "Soria";
		break;
		case 7:
			retVal = "Tarragona";
		break;
		case 8:
			retVal = "Cuenca";
		break;
		case 9:
			retVal = "Huesca";
		break;
	}
	return retVal;
}

int generarPrecio(){
	srand(time(NULL)); 
    int retVal = rand() % 500 + 100;
	return retVal;
}

Tupla generarOferta(int id, int precio){
	srand(time(NULL));

	string origen = getCiudadRandom();
	string destino = getCiudadRandom();
	
	while( origen.compare(destino) == 0 ) destino = getCiudadRandom();
	
	Tupla tupla(to_string(id), origen, destino, to_string(precio));
	return tupla;
}
 
int actualizarContador(LindaDriver& LD){
	Tupla contador("Contador","?X");
	Tupla contadorLeer(2);
	int id;

	LD.RN(contador, ref(contadorLeer));
	id = stoi(contadorLeer.get(1));

	contadorLeer.set(1,to_string(id+1));
	LD.PN(contadorLeer);

	return id;
}

void publicador(string IP, int puerto){

	LindaDriver LD(IP, puerto);	
	int id, precio;
	Tupla tupla(4);
	
	while(true){
		id = actualizarContador(ref(LD));
		precio = generarPrecio();

		tupla.copyFrom(generarOferta(id,precio));
		LD.PN(tupla);
		cout << "PUBLICADO: " <<tupla.to_string()<< endl;

		this_thread::sleep_for(chrono::seconds(1));
	}
	
}

int main(int argc, char* argv[]){
	
	if(argc != 3) {
        cout << "Datos de entrada mal introducidos" << endl;
        cout << "Ejecute como: Publicador *Direccion* *Puerto*" << endl;
        exit(1);
    }

	const int N = 5;
	const int N_CIUDADES = 10;
	thread p[N];
	
	for(int i = 0; i < N; i++){
		p[i] = thread(&publicador,argv[1],atoi(argv[2]));
	}

	for(int i = 0; i < N; i++){
		p[i].join();
	}

	return 0;
}
