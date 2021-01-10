#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define NR_PHILOSOPHERS 5

pthread_t philosophers[NR_PHILOSOPHERS];

//Para mutex
pthread_mutex_t forks[NR_PHILOSOPHERS];

//Para semáforos
sem_t semForks[NR_PHILOSOPHERS];

char* p;

char* names[NR_PHILOSOPHERS] = 
{
    "F. Ranco",
    "F. Raga",
    "M. Rajoy",
    "M. Casas",
    "C. añita Brava"
};

/*void init()
{
    int i;
    for(i=0; i<NR_PHILOSOPHERS; i++)
        pthread_mutex_init(&forks[i],NULL);
    
}*/

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
    int nPhilosopher = (int)i;
    int right = nPhilosopher;
    int left = (nPhilosopher - 1 == -1) ? NR_PHILOSOPHERS - 1 : (nPhilosopher - 1);
    
    while(1) 
    {   
        think(nPhilosopher);
        
        /// TRY TO GRAB BOTH FORKS (right and left)

        //Coger qué tenedores

        int r = right;
        int l = left;

        if (right > left) //Por si casos limite, se invierte
        {
            r = left;
            l = right;
        }

        //Coger tenedores

        pthread_mutex_lock(&forks[r]); 
        pthread_mutex_lock(&forks[l]);


        eat(nPhilosopher);
        
        // PUT FORKS BACK ON THE TABLE

        //Se dejan los tenedores en la mesa

        pthread_mutex_unlock(&forks[l]); 
        pthread_mutex_unlock(&forks[r]);

        toSleep(nPhilosopher);
   }
}

void* philosopherSemaforo(void* i)
{
    int nPhilosopher = (int)i;
    int right = nPhilosopher;
    int left = (nPhilosopher - 1 == -1) ? NR_PHILOSOPHERS - 1 : (nPhilosopher - 1);
    
    while(1) 
    {   
        think(nPhilosopher);
        
        ///TRY TO GRAB BOTH FORKS (right and left)

        //Coger qué tenedores

        int r = right;
        int l = left;

        if (right > left) //Por si casos limite, se invierte
        {
            r = left;
            l = right;
        }

        //Coger tenedores

        sem_wait(&semForks[r]);
        sem_wait(&semForks[l]);

        eat(nPhilosopher); //Esto se lo saltan
        
        //PUT FORKS BACK ON THE TABLE

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
		fprintf(stderr,"Usage: %s <0/1> //0 para mutex, 1 para semaforos \n",argv[0]);
		exit(1);
	}

  int control = strtol(argv[1], p, 10); //De string a int. Que string (argumento 2, el 1 es el nombre del archivo), puntero, base de conversion

    if (control == 0) //Mutex
    {
        printf("Mutex, anda que ya te vale\n");

        //Init
        int i;

        for(i = 0; i<NR_PHILOSOPHERS; i++)
            pthread_mutex_init(&forks[i],NULL);
        
        //Creacion de hilos

        for(i=0; i<NR_PHILOSOPHERS; i++)
            pthread_create(&philosophers[i], NULL, philosopherMutex, (void*)i);
        
        for(i=0; i<NR_PHILOSOPHERS; i++)
            pthread_join(philosophers[i],NULL);
    }
    else if (control == 1) //Semaforos
    {
        printf("Semaforos, vaya tela\n");

        int i;

        sem_init(&semForks, 0, NR_PHILOSOPHERS);
        /*
        init();
        unsigned long i;*/
        
        for(i=0; i<NR_PHILOSOPHERS; i++)
            pthread_create(&philosophers[i], NULL, philosopherSemaforo, (void*)i);
        
        for(i=0; i<NR_PHILOSOPHERS; i++)
            pthread_join(philosophers[i],NULL);
    }
    else
    {
        printf("Eres tontisimo\n");
    }
    
    return 0;
} 
