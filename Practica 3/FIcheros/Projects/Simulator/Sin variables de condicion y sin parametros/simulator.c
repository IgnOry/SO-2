#include <stdlib.h>
#include <stdio.h>
#include <pthread.h> //para pthreads
#include <unistd.h>

//Estado del bus
#define EN_RUTA 0 // autobús en ruta
#define EN_PARADA 1 // autobús en la parada
#define N_PARADAS 5 // número de paradas de la ruta
#define MAX_USUARIOS 40 // capacidad del autobús
#define USUARIOS 4 // numero de usuarios

// estado inicial
int estado = EN_RUTA; // = 0, = 1 para EN_PARADA
int parada_actual = 0; // parada en la que se encuentra el autobus
int n_ocupantes = 0; // ocupantes que tiene el autobús

int esperando_parada[N_PARADAS]; //= {0,0,...0}; // personas que desean subir en cada parada
int esperando_bajar[N_PARADAS]; //= {0,0,...0}; // personas que desean bajar en cada parada

pthread_t bus;
pthread_t usuarios[USUARIOS];
pthread_mutex_t mutex_bus;
pthread_mutex_t mutex_usuarios;

//Resto de definiciones globales (comunicacion y sincronizacion)

void Autobus_En_Parada()
{
	/* Ajustar el estado y bloquear al autobús hasta que no haya pasajeros que
	quieran bajar y/o subir la parada actual. Después se pone en marcha */

	estado = EN_PARADA;
	printf("Autobus en parada %d. Por favor, cuidado al salir\n", parada_actual);
	while(esperando_bajar[parada_actual] != 0 || esperando_parada[parada_actual] != 0 || (esperando_parada[parada_actual] != 0 && n_ocupantes == MAX_USUARIOS))
	{
  		sleep(random() % 2);
	}
}

void Conducir_Hasta_Siguiente_Parada()
{
	/* Establecer un Retardo que simule el trayecto y actualizar numero de parada */

	//conduce
	printf("Conduciendo...\n");
    sleep(random() % 2);
   
   	//Se bloquea el mutex, cambio de estado, llegada a la parada, se desbloquea el mutex

	pthread_mutex_lock(&mutex_bus);

	estado = EN_RUTA;

	//llega a la siguiente parada
	parada_actual = (parada_actual + 1 == N_PARADAS) ? 0: (parada_actual + 1);

 	printf("El autobus ha llegado a la parada %d!!\n", parada_actual);

	pthread_mutex_unlock(&mutex_bus);
}

void Subir_Autobus(int id_usuario, int origen, int destino)
{
	/* El usuario indicará que quiere subir en la parada ’origen’, esperará a que
	el autobús se pare en dicha parada y subirá. El id_usuario puede utilizarse para
	proporcionar información de depuración */

	pthread_mutex_lock(&mutex_usuarios);
	esperando_parada[origen]++;
	pthread_mutex_unlock(&mutex_usuarios);

	int subir = 0;

	while (!subir)
	{
		if ((estado == EN_PARADA && parada_actual == origen) && (n_ocupantes < MAX_USUARIOS))
		{
			printf("El usuario %d va a subir\n", id_usuario);

			//Se bloquea el mutex, subir a true, se reduce numero de pasajeros esperando a subir, se aumenta numero de pasajeros esperando a bajar, se aumenta pasajeros, se desbloquea el mutex
			pthread_mutex_lock(&mutex_usuarios);
			subir = 1; //True
			esperando_parada[origen]--;
			esperando_bajar[destino]++;
			n_ocupantes++;
			pthread_mutex_unlock(&mutex_usuarios);
		}
	}

}

void Bajar_Autobus(int id_usuario, int destino)
{
	/* El usuario indicará que quiere bajar en la parada ’destino’, esperará a que
	el autobús se pare en dicha parada y bajará. El id_usuario puede utilizarse para
	proporcionar información de depuración */

	int bajar = 0; //False

	while (!bajar)
	{
		if (EN_PARADA && parada_actual == destino)
		{
			//Se bloquea el mutex, bajar a true, se reduce numero de pasajeros esperando a bajar, se reduce pasajeros, se desbloquea el mutex

			pthread_mutex_lock(&mutex_usuarios);
			bajar = 1; //True
			esperando_bajar[destino]--;
			n_ocupantes--;
			pthread_mutex_unlock(&mutex_usuarios);
		}
	}
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

	printf("MaxUsuarios %d\n", MAX_USUARIOS);
	printf("Usuarios %d\n", USUARIOS);
	printf("NParadas %d\n", N_PARADAS);

	if (USUARIOS > MAX_USUARIOS)
	{
		fprintf(stderr,"%s","No me pongas mas usuarios que el máximo de usuarios por favor\n");
		exit(EXIT_FAILURE);
	}

	// Crear el thread Autobus
	pthread_create(&bus, NULL, thread_autobus, NULL); //Hilo
    
	//Inicialización de mutex
	pthread_mutex_init(&mutex_usuarios, NULL);
	pthread_mutex_init(&mutex_bus, NULL);

	for (i = 0; i < USUARIOS; i++) 	//Crear thread para los usuarios
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