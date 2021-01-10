#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int N_PARADAS = 5;
#define EN_RUTA 0
#define EN_PARADA 1
int MAX_USUARIOS = 40;
int USUARIOS = 4;

// estado inicial
int estado = EN_RUTA;
int parada_actual = 0; // parada en la que se encuentra el autobus
int n_ocupantes = 0; // ocupantes que tiene el autobús

int* esperando_parada; //= {0,0,...0};
int* esperando_bajar; //= {0,0,...0};

pthread_cond_t busParado;
pthread_cond_t busArranca;

pthread_t* usuarios;
pthread_t bus;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// Otras definiciones globales (comunicación y sincronización)

void Autobus_En_Parada() {
	/* Ajustar el estado y bloquear al autobús hasta que no haya pasajeros que
	quieran bajar y/o subir la parada actual. Después se pone en marcha */
	int i;

	pthread_mutex_lock(&mutex);
	estado = EN_PARADA;

	printf("---\nPARADA %d\n%d suben\n%d bajan\n---\n", parada_actual, esperando_parada[parada_actual], esperando_bajar[parada_actual]);

	pthread_cond_broadcast(&busParado);

	//si hay más ocupantes que los que caben, el bus saldrá sin coger más
	while ((esperando_parada[parada_actual] > 0 || esperando_bajar[parada_actual] > 0) && n_ocupantes <= MAX_USUARIOS) {
		pthread_cond_wait(&busArranca, &mutex);
	}

	printf("Arreando!\n");

	estado = EN_RUTA;

	pthread_mutex_unlock(&mutex);

}
void Conducir_Hasta_Siguiente_Parada() {
	/* Establecer un Retardo que simule el trayecto y actualizar numero de parada */
	parada_actual = (parada_actual == N_PARADAS - 1) ? 0 : parada_actual + 1;

	printf("Siguiente parada: %d\n", parada_actual);
	sleep(rand() % 5);

	estado = EN_PARADA;
}
void Subir_Autobus(int id_usuario, int origen) {
	/* El usuario indicará que quiere subir en la parada ’origen’, esperará a que
	el autobús se pare en dicha parada y subirá. El id_usuario puede utilizarse para
	proporcionar información de depuración */

	pthread_mutex_lock(&mutex);


	esperando_parada[origen] += 1;

	//si hay más ocupantes que los que caben, el bus saldrá sin coger más
	while (n_ocupantes > MAX_USUARIOS || estado != EN_PARADA || parada_actual != origen) {
		pthread_cond_wait(&busParado, &mutex);
	}
	printf("Usuario %d subido: %d\n", id_usuario, parada_actual);

	esperando_parada[origen] -= 1;
	n_ocupantes += 1;
	
	printf("Usuario %d desbloqueado\n", id_usuario);

	pthread_cond_signal(&busArranca);


	pthread_mutex_unlock(&mutex);
}
void Bajar_Autobus(int id_usuario, int destino) {
	/* El usuario indicará que quiere bajar en la parada ’destino’, esperará a que
	el autobús se pare en dicha parada y bajará. El id_usuario puede utilizarse para
	proporcionar información de depuración */
	pthread_mutex_lock(&mutex);


	esperando_bajar[destino] += 1;

	//si hay más ocupantes que los que caben, el bus saldrá sin coger más
	while (n_ocupantes > MAX_USUARIOS || estado != EN_PARADA || parada_actual != destino) {
		pthread_cond_wait(&busParado, &mutex);
	}
	printf("Usuario %d bajado: %d\n", id_usuario, parada_actual);

	esperando_bajar[destino] -= 1;
	n_ocupantes -= 1;

	printf("Usuario %d desbloqueado\n", id_usuario);

	pthread_cond_signal(&busArranca);


	pthread_mutex_unlock(&mutex);
}


void* thread_autobus(void* args) {
	while (1) {
		// esperar a que los viajeros suban y bajen
		Autobus_En_Parada();

		// conducir hasta siguiente parada
		Conducir_Hasta_Siguiente_Parada();
	}
}
void* thread_usuario(void* arg) {
	int id_usuario = (int)arg;
	int a, b;

	while (1) {
		a = rand() % N_PARADAS;
		do {
			b = rand() % N_PARADAS;
		} while (a == b);
		Usuario(id_usuario, a, b);
	}
}
void Usuario(int id_usuario, int origen, int destino) {
	// Esperar a que el autobus esté en parada origen para subir
	Subir_Autobus(id_usuario, origen);


	// Bajarme en estación destino
	Bajar_Autobus(id_usuario, destino);
}

int main(int argc, char* argv[]) {
	int i;
	char* pEnd;
	// Definición de variables locales a main
	// Opcional: obtener de los argumentos del programa la capacidad del
	// autobus, el numero de usuarios y el numero de paradas

	if(argc < 4){
		fprintf(stderr,"%s","Uso: simulator capacidad_autobus num_usuarios num_paradas");
		exit(EXIT_FAILURE);
	}

	MAX_USUARIOS = strtol(argv[1], &pEnd, 10);
	USUARIOS = strtol(argv[2], &pEnd, 10);
	N_PARADAS = strtol(argv[3], &pEnd, 10);


	usuarios = malloc(USUARIOS * sizeof(int));

	esperando_parada = malloc(N_PARADAS * sizeof(int));
	esperando_bajar = malloc(N_PARADAS * sizeof(int));
	 

	// Crear el thread Autobus
    pthread_mutex_init(&mutex, NULL);

	pthread_create(&bus, NULL, thread_autobus, NULL);

	pthread_cond_init(&busParado, NULL);
	pthread_cond_init(&busArranca, NULL);


	for (i = 0; i < USUARIOS; i++) {
		pthread_create(&usuarios[i], NULL, thread_usuario, (void*)i);
	}


	for (i = 0; i < USUARIOS; i++) {
		pthread_join(usuarios[i], NULL);
	}

	return 0;
}
