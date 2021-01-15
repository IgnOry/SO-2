#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define NR_PHILOSOPHERS 5

pthread_t philosophers[NR_PHILOSOPHERS];

pthread_mutex_t forks[NR_PHILOSOPHERS]; //Para mutex

sem_t semForks[NR_PHILOSOPHERS]; //Para semáforos

char* names[NR_PHILOSOPHERS] = 
{
    "Kaidy Kayn",
    "Kidd Keo",
    "Yung Beef",
    "Cecilio G",
    "Pimp Flaco"
};

void think(int i) 
{
    printf("%s thinking... \n" , names[i]);
    sleep(random() % 3);
    printf("%s stopped thinking!!! \n" , names[i]);
}

void eat(int i) 
{
    printf("%s eating... \n" , names[i]);
    sleep(random() % 3);
    printf("%s is not eating anymore!!! \n" , names[i]);
}

void toSleep(int i) 
{
    printf("%s sleeping... \n" , names[i]);
    sleep(random() % 3);
    printf("%s is awake!!! \n" , names[i]);
}

void* philosopherMutex(void* i)
{
    int nPhilosopher = (int) i;
    int right = nPhilosopher;
    int left = (nPhilosopher - 1 == -1) ? NR_PHILOSOPHERS - 1 : (nPhilosopher - 1);
    
    while(1) //true
    {   
        think(nPhilosopher);
        
        //Coger qué tenedores

        int r = right;
        int l = left;

        if (right > left) //Por si casos limite, se invierte
        {
            r = left;
            l = right;
        }

        //Coger los tenedores/mutex seleccionados
        pthread_mutex_lock(&forks[r]); 
        pthread_mutex_lock(&forks[l]);


        eat(nPhilosopher);
        
        //Se dejan los tenedores en la mesa/se liberan mutex
        pthread_mutex_unlock(&forks[l]); 
        pthread_mutex_unlock(&forks[r]);

        toSleep(nPhilosopher);
   }
}

void* philosopherSemaforo(void* i)
{
    int nPhilosopher = (int) i;
    int right = nPhilosopher;
    int left = (nPhilosopher - 1 == -1) ? NR_PHILOSOPHERS - 1 : (nPhilosopher - 1);
    
    while(1) //true
    {   
        think(nPhilosopher);
        
        //Coger qué tenedores

        int r = right;
        int l = left;

        if (right > left) //Por si casos limite, se invierte
        {
            r = left;
            l = right;
        }

        //Coger tenedores/semaforos
        sem_wait(&semForks[r]);
        sem_wait(&semForks[l]);

        eat(nPhilosopher); //Esto se lo saltan
        
        //Se dejan los tenedores en la mesa
        sem_post(&semForks[r]);
        sem_post(&semForks[l]);

        toSleep(nPhilosopher);
   }
}

int main(int argc, char* argv[]) 
{
    if (argc!=2) //Control
    {
		fprintf(stderr,"Uso: %s <0/1> //0 para mutex, 1 para semaforos \n",argv[0]);
		exit(1);
	}

    int i;

    int control = strtol(argv[1], NULL, 10); //De string a int. Que string (argumento 2, el 1 es el nombre del archivo), puntero, base de conversion

    if (control == 0) //Mutex
    {
        printf("Mutex\n");

        //Inicializacion
        for(i = 0; i<NR_PHILOSOPHERS; i++)
        {
           pthread_mutex_init(&forks[i],NULL);
        }

        //Creacion de hilos
        for(i=0; i<NR_PHILOSOPHERS; i++)
        {
           pthread_create(&philosophers[i], NULL, philosopherMutex, (void*)i);
        }

        //Espera de hilos
        for(i=0; i<NR_PHILOSOPHERS; i++)
        {
            pthread_join(philosophers[i],NULL);
        }
    }
    else if (control == 1) //Semaforos
    {
        printf("Semaforos\n");

        //Inicializacion
        for(i = 0; i<NR_PHILOSOPHERS; i++)
        {
            sem_init(&semForks[i], 1, NR_PHILOSOPHERS);
        }

        //Creacion de hilos
        for(i=0; i<NR_PHILOSOPHERS; i++)
        {
            pthread_create(&philosophers[i], NULL, philosopherSemaforo, (void*)i);
        }

        //Espera de hilos
        for(i=0; i<NR_PHILOSOPHERS; i++)
        {
            pthread_join(philosophers[i],NULL);
        }
    }
    else
    {
		fprintf(stderr,"Uso: %s <0/1> //0 para mutex, 1 para semaforos \n",argv[0]);
		exit(1);
	}
    
    return 0;
} 
