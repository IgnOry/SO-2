#include <stdlib.h>
#include <stdio.h>
#include <pthread.h> //para pthreads
#include <unistd.h>

//Estado del bus
#define EN_RUTA 0 // autobús en ruta
#define EN_PARADA 1 // autobús en la parada

//Parametros (por defecto, pero esta puesto para que se coja de la entrada)
int N_PARADAS = 5; // número de paradas de la ruta
int MAX_USUARIOS = 40; // capacidad del autobús
int USUARIOS = 4; // numero de usuarios

// estado inicial
int estado = EN_RUTA; // = 0, = 1 para EN_PARADA
int parada_actual = 0; // parada en la que se encuentra el autobus
int n_ocupantes = 0; // ocupantes que tiene el autobús

//Arrays dinamicos para poder ajustar el tamaño segun los parametros de entrada
int* esperando_parada; //= {0,0,...0}; // personas que desean subir en cada parada
int* esperando_bajar; //= {0,0,...0}; // personas que desean bajar en cada parada

//Variables de condicion
pthread_cond_t busParado;
pthread_cond_t busListo;

//Threads
pthread_t bus;
pthread_t* usuarios; //[USUARIOS]; //Array dinamico para poder ajustar el tamaño segun los parametros de entrada

//Mutex
pthread_mutex_t mutex;

//Resto de definiciones globales (comunicacion y sincronizacion)

void Autobus_En_Parada()
{
	/* Ajustar el estado y bloquear al autobús hasta que no haya pasajeros que
	quieran bajar y/o subir la parada actual. Después se pone en marcha */

   	//Se bloquea el mutex, cambio de estado, llegada a la parada, gestion de condiciones, se desbloquea el mutex

	pthread_mutex_lock(&mutex);
	
	estado = EN_PARADA;
	printf("Autobus en parada %d. Por favor, cuidado al salir\n", parada_actual);

	//espera hasta que todos los usuarios de la parada se suban y los que tengan que bajar se hayan bajado
	while(esperando_bajar[parada_actual] > 0 || (esperando_parada[parada_actual] > 0)&& n_ocupantes < MAX_USUARIOS)
	{
		//avisa a todos los usuarios que esta parado en parada_actual
		pthread_cond_broadcast(&busParado);

		pthread_cond_wait(&busListo, &mutex);
	}

	printf("No more users waiting.\n");

	pthread_mutex_unlock(&mutex);
}

void Conducir_Hasta_Siguiente_Parada()
{
	/* Establecer un Retardo que simule el trayecto y actualizar numero de parada */

	//conduce
	printf("Conduciendo...\n");
    sleep(random() % 2);

   	//Se bloquea el mutex, cambio de estado, llegada a la parada, se desbloquea el mutex

	pthread_mutex_lock(&mutex);

	estado = EN_RUTA;

	//llega a la siguiente parada
	parada_actual = (parada_actual + 1) % N_PARADAS;

 	printf("El autobus ha llegado a la parada %d!!\n", parada_actual);

	pthread_mutex_unlock(&mutex);
}

void Subir_Autobus(int id_usuario, int origen, int destino)
{
	/* El usuario indicará que quiere subir en la parada ’origen’, esperará a que
	el autobús se pare en dicha parada y subirá. El id_usuario puede utilizarse para
	proporcionar información de depuración */

	//Se bloquea el mutex, gestion de condiciones y pasajeros, se desbloquea el mutex
	pthread_mutex_lock(&mutex);
	esperando_parada[origen]++;

	while( n_ocupantes < MAX_USUARIOS  && estado == EN_PARADA && parada_actual != origen)
	{
		pthread_cond_wait(&busParado, &mutex);
	}

	printf("El usuario %d va a subir\n", id_usuario);
	n_ocupantes++;

	esperando_parada[origen]--;

	if(esperando_bajar[origen] == 0 && esperando_parada[origen] == 0)
	{
		pthread_cond_signal(&busListo);
	}

	pthread_mutex_unlock(&mutex);
}

void Bajar_Autobus(int id_usuario, int destino)
{
	/* El usuario indicará que quiere bajar en la parada ’destino’, esperará a que
	el autobús se pare en dicha parada y bajará. El id_usuario puede utilizarse para
	proporcionar información de depuración */

	//Se bloquea el mutex, gestion de condiciones y pasajeros, se desbloquea el mutex
	pthread_mutex_lock(&mutex);

	esperando_bajar[destino]++;

	while( estado == EN_PARADA && parada_actual != destino)
	{
		pthread_cond_wait(&busParado, &mutex);
	}

	printf("User %d gets out the bus.\n", id_usuario);
	n_ocupantes--;

	esperando_bajar[destino]--;

	if(esperando_bajar[destino] == 0 && esperando_parada[destino] == 0)
	{
		pthread_cond_signal(&busListo);
	}

	pthread_mutex_unlock(&mutex);
}


// Otras definiciones globales (comunicación y sincronización)
void * thread_autobus(void * args) 
{
	while (1) 
	{
		Autobus_En_Parada(); //esperar a que los viajeros suban y bajen
		Conducir_Hasta_Siguiente_Parada(); //conducir hasta siguiente parada
	}
}

void * thread_usuario(void * arg)
{
	int id_usuario = (int) arg; // obtener el id del usario
	int a,b;

	while (1) 
	{
		a=rand() % N_PARADAS;
		
		do
		{
			b=rand() % N_PARADAS;
		} 
		while(a==b);
		
		printf("El usuario %d esta en la parada %d y va a la parada %d\n", id_usuario, a, b);

		Usuario(id_usuario,a,b);
	}
}
void Usuario(int id_usuario, int origen, int destino) 
{
	Subir_Autobus(id_usuario, origen, destino); //Esperar a que el autobus esté en parada origen para subir
	Bajar_Autobus(id_usuario, destino); //Bajarme en estación destino
}

int main(int argc, char *argv[]) 
{
	int i;

	if(argc < 4){
		fprintf(stderr,"%s","./simulator capacidad_autobus num_usuarios num_paradas");
		exit(EXIT_FAILURE);
	}

	MAX_USUARIOS = strtol(argv[1], NULL, 10);
	USUARIOS = strtol(argv[2], NULL, 10);
	N_PARADAS = strtol(argv[3], NULL, 10);

	printf("MaxUsuarios %d\n", MAX_USUARIOS);
	printf("Usuarios %d\n", USUARIOS);
	printf("NParadas %d\n", N_PARADAS);

	if (USUARIOS > MAX_USUARIOS) //Debug
	{
		fprintf(stderr,"%s","No me pongas mas usuarios que el máximo de usuarios por favor\n");
		exit(EXIT_FAILURE);
	}

	//Creacion de arrays
	usuarios = malloc(USUARIOS * sizeof(int));
	esperando_parada = malloc(N_PARADAS * sizeof(int));
	esperando_bajar = malloc(N_PARADAS * sizeof(int));

	// Crear el thread Autobus
	pthread_create(&bus, NULL, thread_autobus, NULL); //Hilo

	//Inicializaciones (se podria sacar a metodo)
	pthread_mutex_init(&mutex, NULL); //Condicion de parada del bus
	pthread_cond_init(&busParado, NULL); //Condicion de arranque del bus
	pthread_cond_init(&busListo, NULL); //Mutex

	for (i = 0; i < USUARIOS; i++) 	//Crear thread para el usuario i
	{
		pthread_create(&usuarios[i], NULL, thread_usuario, (void*)i);
	}

	pthread_join(bus, NULL);

	for (i = 0; i < USUARIOS; i++) 	//Esperar terminación de los hilos
	{
		pthread_join(usuarios[i], NULL);
	}

	return 0;
}