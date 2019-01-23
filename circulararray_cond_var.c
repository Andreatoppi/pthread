#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define N 10

// int array[10];


struct array_c {
    
    int array[10];

    int head, tail, num_e;

    pthread_cond_t full, empty;

    pthread_mutex_t mutex;
    
} array;

void init_array_c (struct array_c *a){

    a->head = a->tail = a->num_e = 0;
    for (int i = 0; i<10; i++)
        a->array[i] = 0;
    // a->array[10] = {0,0,0,0,0,0,0,0,0,0};
    pthread_mutex_init (&a->mutex, NULL);

    pthread_cond_init (&a->full, NULL);
    pthread_cond_init (&a->empty, NULL);

}

void *inserisci (void *arg){

    struct array_c *a = &array;
    
    int elem = rand() % 9 + 1;

    pthread_mutex_lock (&a->mutex);

    while (a->num_e == 10){
        printf("I :mi appendo\n");
        pthread_cond_wait (&a->full, &a->mutex);
    }
    a->array[a->head] = elem;
    a->head = (a->head+1)%10;
    a->num_e++;

    printf ("I : ho inserito : %d n_elem = %d, head = %d, tail = %d l'array è: ", elem, a->num_e, a->head, a->tail );
    for (int i=0; i<10; i++)
        printf ("%d", a->array[i]);
    printf ("\n");
    pthread_mutex_unlock (&a->mutex);
    if (a->num_e == 1)
        pthread_cond_signal (&a->empty);

}

void *estrai (void *arg){

    struct array_c *a = &array;
    
    pthread_mutex_lock (&a->mutex);

    int elem = 0;

    while (a->num_e == 0){
        printf ("E: mi appendo\n");
        pthread_cond_wait (&a->empty, &a->mutex);
    }
    elem = a->array[a->tail];
    a->array[a->tail] = 0;
    a->tail = (a->tail+1)%10;
    a->num_e--;

    printf ("E : ho estratto : %d ,n_elem = %d, head = %d, tail = %d l'array è: ", elem, a->num_e, a->head, a->tail );
    for (int i=0; i<10; i++)
        printf ("%d", a->array[i]);

    printf ("\n");

    pthread_mutex_unlock (&a->mutex);
    if (a->num_e == 9)
        pthread_cond_signal (&a->empty);

}


int main(int argc, char const *argv[])
{
    int t = 0;
    struct array_c *a = &array;

    pthread_t inseritore, estrattore;

    init_array_c (&array);

    srand(time(NULL));

    for (int i=0; i<50; i++){
        t = rand() % 10 + 1;
        if (t > 8)
            pthread_create (&inseritore, NULL, inserisci, NULL);
        else
            pthread_create (&estrattore, NULL, estrai, NULL);

        sleep (1);

    }
    pthread_join (inseritore, NULL);
    pthread_join (estrattore, NULL);

    return 0;
}
