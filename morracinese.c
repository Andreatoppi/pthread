#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define CARTA 0
#define SASSO 1
#define FORBICE 2

char *nomi_mosse[3] = {"carta", "sasso", "forbice"};
int scelta11, scelta21;

void *th_giocatore1 (void *arg){
    int * scelta1 = &scelta11;
    
    *scelta1 = rand()%3+1;
    printf ("TH1 : Butto: %s\n", nomi_mosse[*scelta1-1]);
    pthread_exit(scelta1);

}

void *th_giocatore2 (void *arg){
    int * scelta2 = &scelta21;
    
    *scelta2 = rand()%3+ 1;
    printf ("TH2 : Butto: %s\n", nomi_mosse[*scelta2-1]);

    pthread_exit(scelta2);
}

void *th_arbitro (void *arg){
    pthread_t giocatore1, giocatore2;

    int *scelta_th1;
    int *scelta_th2;

    printf ("comincia la partita, fate il vostro gioco! \n");

    pthread_create (&giocatore1, NULL, th_giocatore1, NULL);
    pthread_create (&giocatore2, NULL, th_giocatore2, NULL);

    pthread_join (giocatore1, (void **)&scelta_th1);
    pthread_join (giocatore2, (void **)&scelta_th2);

    if (nomi_mosse[*scelta_th1-1] == nomi_mosse[*scelta_th2-1])
        printf ("A : partita patta\n");
    else if (nomi_mosse[*scelta_th1-1] == "sasso" && nomi_mosse[*scelta_th2-1] == "forbice")
            printf("A : Vince TH1\n");
        else if (nomi_mosse[*scelta_th1-1] == "carta" && nomi_mosse[*scelta_th2-1] == "sasso")
            printf("A : Vince TH1\n");
            else if (nomi_mosse[*scelta_th1-1] == "forbice" && nomi_mosse[*scelta_th2-1] == "carta")
                    printf ("A : Vince TH1\n");
                else
                    printf("A : Vince TH2\n");
}


int main (){

    pthread_t arbitro;

    srand(time(NULL));

    pthread_create (&arbitro, NULL, th_arbitro, NULL);

    pthread_join (arbitro, NULL);

    return 0;
}