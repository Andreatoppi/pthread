#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


#define N 5
#define DELAY 3

sem_t forchetta[N];
pthread_mutex_t mutex;

int id = 0; 

void pensa(int i){
    printf ("Sono il filosofo %d e penso\n", i);

    sleep(DELAY);
}

void mangia(int i){
    int k;
    if (i == 0){
        k = 4;
    }
    else
        k = i-1;

    sem_wait (&forchetta[i]);
    sem_wait (&forchetta[k]);
        printf ("Sono il filosofo %d e mangio\n", i);
        sleep(DELAY);
    sem_post (&forchetta[i]);
    sem_post (&forchetta[k]);

}

void *filosofo (void *arg){
    int index;
    
    pthread_mutex_lock(&mutex);
        index = id;
        id++;
    pthread_mutex_unlock(&mutex);

    for (;;){

    pensa(index);

    mangia(index);

    }
    //printf ("%d : Ho le due forchette e posso mangiare!!\n", index);


}

int main(int argc, char const *argv[])
{
    pthread_t filosofo0, filosofo1, filosofo2, filosofo3, filosofo4;
   
    for (int i=0; i<N; i++)
        sem_init (&forchetta[i], 0, 1);

    pthread_mutex_init (&mutex, NULL);

    pthread_create (&filosofo0, NULL, filosofo, NULL);
    pthread_create (&filosofo1, NULL, filosofo, NULL);
    pthread_create (&filosofo2, NULL, filosofo, NULL);
    pthread_create (&filosofo3, NULL, filosofo, NULL);
    pthread_create (&filosofo4, NULL, filosofo, NULL);
    
    pthread_join (filosofo0, NULL);
    pthread_join (filosofo1, NULL);
    pthread_join (filosofo2, NULL);
    pthread_join (filosofo3, NULL);
    pthread_join (filosofo4, NULL);

    return 0;
}
