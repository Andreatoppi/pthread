#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define N 5

int scelta = 0;


struct gestore_t {

    sem_t R_A, R_B, R_A_o_B;
    pthread_mutex_t mutex;

    int ca, cb;

}gestore;

void init_gestore_t (struct gestore_t *g){

    sem_init(&g->R_A, 0, 1);
    sem_init(&g->R_B, 0, 1);
    sem_init(&g->R_A_o_B, 0, 2);

    pthread_mutex_init (&g->mutex, NULL);

    g->ca = g->cb = 0;
}

void richA (struct gestore_t *g, int i){
    printf ("th_%d : Ho bisogno della risorsa A \n", i);
   
    sem_wait (&g->R_A);
    sem_wait (&g->R_A_o_B);
        g->ca++;
    printf ("th_%d : Ho ottenuto la risorsa A e la blocco per un secondo \n", i);
    sleep(1);
        g->ca--;
    sem_post (&g->R_A);
    sem_post (&g->R_A_o_B);
}

void richB (struct gestore_t *g, int i){
    printf ("th_%d : Ho bisogno della risorsa B \n", i);

    sem_wait (&g->R_B);
    sem_wait (&g->R_A_o_B);
        g->cb++;
    printf ("th_%d : Ho ottenuto la risorsa B e la blocco per un secondo \n", i);
    sleep(1);
        g->cb--;
    sem_post (&g->R_B);
    sem_post (&g->R_A_o_B);

}

void rich2 (struct gestore_t *g, int i){
    printf ("th_%d : Ho bisogno della risorsa A e B\n", i);
    
    sem_wait (&g->R_A);
    sem_wait (&g->R_B);
        g->ca++;
        g->cb++;
    printf ("th_%d : Ho ottenuto la risorsa A e B e le blocco per un secondo \n", i);
    sleep(1);
        g->ca--;
        g->cb--;
    sem_post (&g->R_A);
    sem_post (&g->R_B);
}

void richQ (struct gestore_t *g, int i){
    printf ("th_%d : Ho bisogno della risorsa A o B\n", i);
    sem_wait (&g->R_A_o_B);
    if (g->ca == 0){
        sem_wait (&g->R_A);
        g->ca++;
        printf ("th_%d : Ho ottenuto la risorsa A e le blocco per un secondo \n", i);
        sleep(1);
        g->ca--;
        sem_post (&g->R_A);
        sem_post (&g->R_A_o_B);
    }
    else{
        sem_wait (&g->R_B);
        g->cb++;
        printf ("th_%d : Ho ottenuto la risorsa B e le blocco per un secondo \n", i);
        sleep(1);
        g->cb--;
        sem_post (&g->R_B);
        sem_post (&g->R_A_o_B);
    }
}

void *processo (void *arg){

    struct gestore_t *g = &gestore;

    int i = (intptr_t)arg;

    pthread_mutex_lock (&g->mutex);
    scelta = rand() % 4 + 1;
    if (scelta == 1){
        pthread_mutex_unlock (&g->mutex);
        richA(&gestore, i);
    }
    else if (scelta == 2){
            pthread_mutex_unlock (&g->mutex);
            richB(&gestore, i);
        }
        else if (scelta == 3){
                pthread_mutex_unlock (&g->mutex);
                rich2(&gestore, i);
            }
            else{
                pthread_mutex_unlock (&g->mutex);
                richQ(&gestore, i);
            }
}

int main(){

    pthread_t proc[N];
    int id = 0;

    init_gestore_t (&gestore);
    struct gestore_t *g = &gestore;


    srand(time(NULL));

    for (int i=0; i<N; i++){
        pthread_create (&proc[i], NULL, processo, (void *)(intptr_t)id);
        id++;
    }

    pthread_join (proc[0], NULL);
    pthread_join (proc[1], NULL);
    pthread_join (proc[2], NULL);
    pthread_join (proc[3], NULL);
    pthread_join (proc[4], NULL);
    
    return 0;
}
