#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NR_PHILOSOPHERS 5

pthread_t philosophers[NR_PHILOSOPHERS];
pthread_mutex_t mutex[NR_PHILOSOPHERS];
static int turn = 0;
//variable estática que determina el turno de cada filósofo

char* names[NR_PHILOSOPHERS] = {"Karl Marx",
"Barnett Newmann",
"Mao Zedong",
"Martin Heidegger",
"Marta Harnecker"};

void init()
{
    int i;

	for (i = 0; i < NR_PHILOSOPHERS; i++)
		pthread_mutex_init(&mutex[i], NULL);

    
}

void think(int i) {
    printf("%s thinking... \n", names[i]);
    sleep(random() % 10);
    printf("%s stopped thinking!!! \n", names[i]);

}

void eat(int i) {
    printf("%s eating... \n", names[i]);
    sleep(random() % 5);
    printf("%s is not eating anymore!!! \n", names[i]);

}

void toSleep(int i) {
    printf("%s sleeping... \n", names[i]);
    sleep(random() % 10);
    printf("%s is awake!!! \n", names[i]);
}

void* philosopher(void* i)
{
    int nPhilosopher = (int)i;
    int right = nPhilosopher;
    int left = (nPhilosopher - 1 == -1) ? NR_PHILOSOPHERS - 1 : (nPhilosopher - 1);
	int waitingTime = 0;
    while(1)
    {
        
        think(nPhilosopher);

        int r = right, l = left;

        if(right > left){
        	r = left;
        	l = right;
        }

		pthread_mutex_lock(&mutex[r]);
		pthread_mutex_lock(&mutex[l]);

		//con 5 mutex, no condiciones

        eat(nPhilosopher);

		pthread_mutex_unlock(&mutex[l]);
		pthread_mutex_unlock(&mutex[r]);

		turn = (turn == 0) ? NR_PHILOSOPHERS - 1 : (turn - 1);	//se avanza el turno
        
        toSleep(nPhilosopher);
   }

}

int main()
{
    init();
    unsigned long i;
    for(i=0; i < NR_PHILOSOPHERS; i++)
        pthread_create(&philosophers[i], NULL, philosopher, (void*)i);
    
    for(i=0; i<NR_PHILOSOPHERS; i++)
        pthread_join(philosophers[i],NULL);
    
    return 0;
} 
