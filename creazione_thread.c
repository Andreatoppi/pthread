#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>


int conto = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread1 (void *arg){
    pthread_mutex_lock (&mutex);
        printf ("Thread 1 : il conto prima dell'incremento e': %d\n", conto);
        conto++;
        printf ("Thread 1 : il conto dopo dell'incremento e': %d\n", conto);
    pthread_mutex_unlock (&mutex);
}

void *thread2 (void *arg){
    int scelta=0;
    pthread_mutex_lock (&mutex);
        printf ("Thread 2 : il conto prima dell'incremento e': %d\n", conto);
        conto++;
        printf ("Thread 2 : il conto dopo dell'incremento e': %d\n", conto);
        scelta = rand() % 3 + 1;
        printf ("scelta %d", scelta);
    pthread_mutex_unlock (&mutex);
}

int main (){

    pthread_t th1, th2;
    int i = 0;

    while (i<10){

        pthread_create (&th1, NULL, thread1, NULL);
        pthread_create (&th2, NULL, thread2, NULL);
        i++;
    }

    pthread_join (th1, NULL);
    pthread_join (th2, NULL);

    return 0;
}