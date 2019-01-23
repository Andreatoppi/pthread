#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>


#define STATO_NESSUNO 0
#define STATO_A 1
#define STATO_B 2
#define STATO_C 3
#define STATO_D 4
#define STATO_E 5
#define STATO_D_o_E 6

struct gestore_t { 

    int na_block, nb_block, nc_block, nd_block, ne_block;
    int na, nb, nc, nd, ne;
    int stato;
    
    sem_t S_a, S_b, S_c, S_d, S_e;

    pthread_cond_t C_a, C_b, C_c, C_d, C_e;

    pthread_mutex_t M_1;

}gestore;

void init_gestore_t (struct gestore_t *g){
    
    g->na = g->nb = g->nc = g->nd = g->ne = 0;
    g->na_block = g->nb_block = g->nc_block = g->nd_block = g->ne_block = 0;

    g->stato = STATO_NESSUNO;

    sem_init (&g->S_a, 0, 0);
    sem_init (&g->S_b, 0, 0);
    sem_init (&g->S_c, 0, 0);
    sem_init (&g->S_d, 0, 0);
    sem_init (&g->S_e, 0, 0);

    pthread_mutex_init (&g->M_1, NULL);

    pthread_cond_init (&g->C_a, NULL);
    pthread_cond_init (&g->C_b, NULL);
    pthread_cond_init (&g->C_c, NULL);
    pthread_cond_init (&g->C_d, NULL);
    pthread_cond_init (&g->C_e, NULL);
}

void startA(struct gestore_t *g){

    pthread_mutex_lock(&g->M_1);

    while (g->stato != STATO_NESSUNO){
        g->na_block++;
        pthread_cond_wait (&g->C_a, &g->M_1);
        g->stato = STATO_NESSUNO;
        // g->na_block--;
    }
    g->na++;
    // g->stato = STATO_A;
    pthread_mutex_unlock(&g->M_1);

}

void eseguiA(){

    printf ("  A\n");
    //sleep(1.5);

}

void endA(struct gestore_t *g){

    pthread_mutex_lock(&g->M_1);

    g->stato = STATO_B;

    while (g->nb_block > 0){
        g->nb_block--;
        // g->nb++;
        pthread_cond_signal(&g->C_b);
    }
    g->na--;

    pthread_mutex_unlock(&g->M_1);

}

void startB(struct gestore_t *g){

    pthread_mutex_lock(&g->M_1);
    
    while (g->stato != STATO_B){
        g->nb_block++;
        pthread_cond_wait(&g->C_b, &g->M_1);
        //g->stato = STATO_B;
    }
    g->nb++;
    // g->stato = STATO_B;
    pthread_mutex_unlock(&g->M_1);
    // sem_wait(&g->S_b);
}

void eseguiB(){

    printf ("  B\n");
    //sleep(1.5);

}

void endB(struct gestore_t *g){

    pthread_mutex_lock(&g->M_1);

    while (g->nb_block > 0){
        g->nb_block--;
        pthread_cond_signal(&g->C_b);
    }

    if (g->na_block != 0){
        g->na_block--;
        // g->na++;
        g->stato = STATO_A;
        pthread_cond_signal (&g->C_a);
    } else if (g->nc_block > 0){
        g->nc_block--;
        // g->nc++;
        g->stato = STATO_C;
        pthread_cond_signal (&g->C_c);
    } else 
        g->stato = STATO_NESSUNO;

    g->nb--;

    pthread_mutex_unlock(&g->M_1);

}

void startC(struct gestore_t *g){

    pthread_mutex_lock(&g->M_1);
    
        while (g->stato != STATO_NESSUNO){
            g->nc_block++;
            pthread_cond_wait(&g->C_c, &g->M_1);
            g->stato = STATO_NESSUNO;
            // g->stato = STATO_C;
        }
        g->nc++;
        // g->stato = STATO_C;
    
    pthread_mutex_unlock(&g->M_1);
    // sem_wait(&g->S_c);
}

void eseguiC(){

    printf ("  C\n");
    //sleep(1.5);

}

void endC(struct gestore_t *g){

    pthread_mutex_lock(&g->M_1);

    g->stato = STATO_D_o_E;

    if (g->nd_block > 0){
        g->nd_block--;
        // g->nd++;
        // g->stato = STATO_D;
        pthread_cond_signal(&g->C_d);
    } else if (g->ne_block > 0){
        g->ne_block--;
        // g->ne++;
        // g->stato = STATO_E;
        pthread_cond_signal(&g->C_e);
    }

    g->nc--;

    pthread_mutex_unlock(&g->M_1);

}

void startD(struct gestore_t *g){

    pthread_mutex_lock(&g->M_1);
    
        while (g->stato != STATO_D_o_E){
            g->nd_block++;
            pthread_cond_wait(&g->C_d, &g->M_1);
            // g->stato = STATO_D;
        }
    
    g->nd++;
    // g->stato = STATO_D;
    
    pthread_mutex_unlock(&g->M_1);
    // sem_wait(&g->S_d);
}

void eseguiD(){

    printf ("  D\n");
    //sleep(1.5);

}

void endD(struct gestore_t *g){

    pthread_mutex_lock(&g->M_1);

    if (g->na_block > 0){
        g->na_block--;
        // g->na++;
        g->stato = STATO_A;
        pthread_cond_signal (&g->C_a);
    } else if (g->nc_block > 0){
        g->nc_block--;
        // g->nc++;
        g->stato = STATO_C;
        pthread_cond_signal (&g->C_c);
    } else 
        g->stato = STATO_NESSUNO;

    g->nd--;

    pthread_mutex_unlock(&g->M_1);

}

void startE(struct gestore_t *g){

    pthread_mutex_lock(&g->M_1);
    
        if (g->stato != STATO_D_o_E){
            g->ne_block++;
            pthread_cond_wait(&g->C_e, &g->M_1);
            // g->stato = STATO_E;
        }
    g->ne++;
    // g->stato = STATO_E;

    pthread_mutex_unlock(&g->M_1);
    // sem_wait(&g->S_e);
}

void eseguiE(){

    printf ("  E\n");
    //sleep(1.5);

}

void endE(struct gestore_t *g){

    pthread_mutex_lock(&g->M_1);

    if (g->na_block > 0){
        g->na_block--;
        // g->na++;
        g->stato = STATO_A;
        pthread_cond_signal (&g->C_a);
    } else if (g->nc_block > 0){
        g->nc_block--;
        // g->nc++;
        g->stato = STATO_C;
        pthread_cond_signal (&g->C_c);
    } else 
        g->stato = STATO_NESSUNO;

    g->ne--;

    pthread_mutex_unlock(&g->M_1);

}

void *proc_A (void *args){

    startA(&gestore);    

    eseguiA();

    endA(&gestore);
}

void *proc_B (void *args){

    startB(&gestore);    

    eseguiB();

    endB(&gestore);
}

void *proc_C (void *args){

    startC(&gestore);    

    eseguiC();

    endC(&gestore);
}

void *proc_D (void *args){

    startD(&gestore);    

    eseguiD();

    endD(&gestore);
}

void *proc_E (void *args){

    startE(&gestore);    

    eseguiE();

    endE(&gestore);
}

int main(){

    srand(time(NULL));

    pthread_t proc_a, proc_b, proc_c, proc_d, proc_e;

    int t = 0;
    int i = 0;

    init_gestore_t(&gestore);

    while (i<20){
        t = rand()%5+1; 
        printf ("  arriva: %d\n", t);
        if (t == 1)
            pthread_create (&proc_a, NULL, proc_A, NULL);
        else if (t == 2)
                pthread_create (&proc_b, NULL, proc_B, NULL);
            else if (t == 3)
                    pthread_create (&proc_c, NULL, proc_C, NULL);
                else if (t == 4)
                        pthread_create (&proc_d, NULL, proc_D, NULL);
                    else if (t == 5)
                            pthread_create (&proc_e, NULL, proc_E, NULL);
        
        sleep(1);
        
        i++;
    }

    //pthread_create (&proc_a, NULL, proc_A, NULL);
    //pthread_join(proc_a, NULL);
    //pthread_join(proc_b, NULL);
    //pthread_join(proc_c, NULL);
    //pthread_join(proc_d, NULL);
    //pthread_join(proc_e, NULL);

    printf ("FINE PROGRAMMA_GENERATI 20 THREAD RANDOM \n");

    return 0;
}