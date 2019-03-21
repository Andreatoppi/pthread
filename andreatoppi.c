#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define N 5                 //numero di clienti massimo

int n_pizze[5] = {6, 6, 6, 6, 6};   //inizializzo il vettore degli ordini a 6, considero 5 come numero di pizze massimo in un orine

struct codaclienti_t {

    pthread_mutex_t mutex;

    pthread_cond_t C_p;     //condition variable del pizzaiolo
    pthread_cond_t C_c[N];  //N condition variable, una per cliente

    int n_clienti;

}coda;

void init_codaclienti_t (struct codaclienti_t *c){

    pthread_mutex_init (&c->mutex, NULL);       //inizializzo il mutex al valore di default 1
    
    pthread_cond_init (&c->C_p, NULL);          //inizializzo le condition variable
    
    for (int i=0; i<N; i++)
        pthread_cond_init (&c->C_c[i], NULL);

    c->n_clienti = 0;
}

void ordina_pizza (struct codaclienti_t *c, int n_p, int cliente){
    n_pizze[cliente] = n_p;
    printf ("C : Sono il cliente %d e ordino %d pizze. \n", cliente, n_p);
    if (c->n_clienti == 1)
        pthread_cond_signal (&c->C_p);
}

void ritira_pizza (struct codaclienti_t *c, int cliente){
    printf ("C : Sono il cliente %d e ritiro le pizze. \n\n", cliente);
    c->n_clienti--;
    n_pizze[cliente] = 6;
}

void prossima_pizza(struct codaclienti_t *c, int j, int cl){
    printf ("P : Faccio la pizza %d per il cliente %d\n", j+1, cl);
    sleep(1);
}

void consegna_pizza (struct codaclienti_t *c, int j, int cl){
    printf ("P : Consegno la pizza %d per il cliente %d\n", j+1, cl);
    sleep(1);
}

void *th_cliente (void *arg){
    struct codaclienti_t *c = &coda;
    while(1){
        sleep(1);

        pthread_mutex_lock (&c->mutex);                 
        c->n_clienti++;                 
        int n_p = rand() % 5 + 1;       //scelta del numero di pizze random

        ordina_pizza (&coda, n_p, (intptr_t)arg);

        pthread_cond_wait (&c->C_c[(intptr_t)arg], &c->mutex);        //aspetto che il mio ordine sia svolto

        ritira_pizza (&coda, (intptr_t)arg);

        pthread_mutex_unlock (&c->mutex);

    }
}

void *th_pizzaiolo (void *arg){
    struct codaclienti_t *c = &coda;
    while(1){
        sleep (1);                      //sleep per evitare che sia sempre il pizzaiolo ad ottenere il mutex
        pthread_mutex_lock (&c->mutex);

        while (c->n_clienti == 0){
            printf ("P : Niente lavoro!! \n");
            pthread_cond_wait (&c->C_p, &c->mutex);     //se non ci sono clienti nella pizzeria non aspetto senza fare niente
        }

        int cliente;
        int n_p = 6;

        for (int i=0; i<N; i++){
            if (n_pizze[i]<n_p){        //cerco l'ordine più piccolo
                n_p = n_pizze[i];
                cliente = i;            //qualche correzione indici rispetto al compito
            }
        }
        
        for (int j=0; j<n_p; j++){
            prossima_pizza(&coda, j, cliente);      //svolgo l'ordine
            consegna_pizza(&coda, j, cliente);      //parametri aggiunti per la stampa
        }

        printf("P : Le pizze sono tutte pronte\n");

        pthread_cond_signal (&c->C_c[cliente]);     //il cliente può essere sbloccato perche il suo ordine è completo
        
        pthread_mutex_unlock (&c->mutex);
    }
}

int main(){

    pthread_t cliente[5];       //creo i thread

    pthread_t pizzaiolo;

    init_codaclienti_t(&coda);      //init della struct

    srand(time(NULL));      //funzione di appoggio per numeri random
    int id_cliente;

    pthread_create (&pizzaiolo, NULL, th_pizzaiolo, NULL);     //creo thread pizzaiolo
/*
Ho simulato che i clienti arrivino 1 ogni secondo.
Togliendo la sleep arrivano tutti insieme e verra soddisfatto prima quello con l'ordine piu piccolo di tutti
*/
    for (int i=0; i<N; i++){
        // id_cliente = rand()%5;        //arriva un cliente random
        pthread_create (&cliente[i], NULL, th_cliente, (void *)(intptr_t)i);
        sleep(1);                       //arriva un cliente al secondo
    }

    pthread_join (pizzaiolo, NULL);
    for (int i=0; i<N; i++)
        pthread_join (cliente[i], NULL);

    return 0;
}