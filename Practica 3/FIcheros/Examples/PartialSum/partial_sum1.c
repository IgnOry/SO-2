#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

int total_sum = 0;
char* p;
sem_t sem;

void * partial_sum(void * arg) 
{
  int ni=((int*)arg)[0];
  int nf=((int*)arg)[1];

  sem_wait(&sem);
  
  printf("[0]: %d, [1]: %d\n",ni, nf);

  //Seccion critica
  for (int j = ni; j <= nf; j++)
  {
    total_sum = total_sum + j;
  }
  printf("total_sum: %d\n",total_sum);

  sem_post(&sem);


  //Seccion critica
  pthread_exit(0);
}

int main(int argc, char* argv[]) 
{
	if (argc!=3) //num_hilos valor_maximo
  {
		fprintf(stderr,"Usage: %s <num_hilos> <valor_maximo> \n",argv[0]);
		exit(1);
	}


  //Variables
  int num_hilos = strtol(argv[1], p, 10); //De string a int. Que string (argumento 2, el 1 es el nombre del archivo), puntero, base de conversion
  int valor_maximo = strtol(argv[2], p, 10); //De string a int. Que string (argumento 2, el 1 es el nombre del archivo), puntero, base de conversion
  
  sem_init(&sem, 0, 1);
  pthread_t threads[num_hilos];
  int nums [num_hilos][2]; 

  for (int i = 0; i < num_hilos; i++)
  {
    if (i == 0) //Primero
      nums[i][0] = 0;
    else
      nums[i][0] = i * (valor_maximo/num_hilos);

    if (i == num_hilos - 1) //Ultimo
      nums[i][1] = valor_maximo;
    else
    {
      nums[i][1] = (i + 1) * (valor_maximo/num_hilos) - 1;
    } 
  }

  /*for (int i = 0; i < num_hilos; i++) //Test
  {
    printf( "%d: [0]: %d, [1]: %d \n", i, nums[i][0], nums[i][1]);
  }
  */

 
  for (int i = 0; i < num_hilos; i++) //Creacion de hilos
  {
    pthread_create(&threads[i], NULL, partial_sum, (void*)nums[i]); //th1 seria th[i], num1 seria num[i]
  }

  for (int i = 0; i < num_hilos; i++) //Espera de hilos
  {
    pthread_join(threads[i], NULL);
  }

 long long int total_teorico = ((valor_maximo * (valor_maximo+1))/2); //A veces se ralla, por tipo??
  printf("total_sum=%d and it should be %llu\n", total_sum, total_teorico);

  return 0;
}
