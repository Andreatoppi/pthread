#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

struct buffer_t {
    int nr, nw, nrb, nwb;
    int buff;

    sem_t S_R, S_W;
    pthread_mutex_t mutex;

} buffer;

void init_buffer_t (struct buffer_t *b){
    b->nr = b->nw = b->nwb = b->nrb = 0;
    b->buff = 0;

    sem_init (&b->S_R, 0, 0);
    sem_init (&b->S_W, 0, 0);

    pthread_mutex_init (&b->mutex, NULL);

}

void leggi(struct buffer_t *b){
    printf ("R: Sto leggendo il buffer, attualmente e: %d\n", b->buff);
    sleep(3);
}

void *lettore (void *args){

    struct buffer_t *b = &buffer;

    pthread_mutex_lock (&b->mutex);

    if (b->nw > 0 || b->nwb > 0){
        b->nrb++;
    }
    else{
        b->nr++;
        sem_post (&b->S_R);
    }
    
    pthread_mutex_unlock (&b->mutex);
    sem_wait (&b->S_R);

    leggi(&buffer);

    pthread_mutex_lock (&b->mutex);
    
    b->nr--;
    if (b->nwb > 0 && b->nr == 0){
        b->nwb--;
        b->nw++;
        sem_post (&b->S_W);
    }
    pthread_mutex_unlock(&b->mutex);

}

void scrivi (struct buffer_t *b){
    int t = rand() % 10 + 1;
    
    printf ("W: Scrivo nel buffer per %d", t);
    printf ("secondi\n");

    while (t != 0){
        b->buff++;    
        t--;
        printf ("W: Valore del buffer: %d\n", b->buff);
        sleep(1);
    }
}

void *scrittore (void *args){

    struct buffer_t *b = &buffer;

    pthread_mutex_lock (&b->mutex);

        if (b->nr > 0 || b->nw > 0){
            b->nwb++;         
        }
        else{
            b->nw++;
            sem_post (&b->S_W);
        }
        
        pthread_mutex_unlock (&b->mutex);
        sem_wait (&b->S_W);

        scrivi(&buffer);

        pthread_mutex_lock (&b->mutex);
        
        b->nw--;
        if (b->nwb > 0){
            b->nwb--;
            b->nw++;
            sem_post (&b->S_W);
        }
        else if (b->nrb > 0){
            b->nrb--;
            b->nr++;
            sem_post (&b->S_R);
        }
        pthread_mutex_unlock (&b->mutex);
}

int main(){
    srand(time(NULL));

    pthread_t th_lettore, th_scrittore;

    init_buffer_t(&buffer);
    struct buffer_t *b = &buffer;
    
    int scelta = 0;
    int i = 0;

    while (i<10){
        scelta = rand() % 4 + 1;
        if (scelta == 1){
            printf ("\nArrivato un Reader\n");
            pthread_create (&th_lettore, NULL, lettore, NULL);
        }
         else if (scelta == 2){
            printf ("\nArrivato un Writer\n");
            pthread_create (&th_scrittore, NULL, scrittore, NULL);
         }
         else
            printf ("\nfree\n");

         i++;
         sleep(1);
    }

    printf ("FINE PROGRAMMA\n");
}