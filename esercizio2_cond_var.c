#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

pthread_cond_t C_ab, C_r;
pthread_mutex_t mutex;

int c_ab, c_r;
int c_ab_b, c_r_b;

void start_ab(){
    pthread_mutex_lock (&mutex);
    while (c_r != 0 || c_r_b != 0){
        c_ab_b++;
        pthread_cond_wait (&C_ab, &mutex);
        c_ab_b--;
    }
    c_ab++;
    pthread_mutex_unlock (&mutex);
}

void esegui_ab(){
    printf ("ab");
}

void end_ab (){
    pthread_mutex_lock(&mutex);
    c_ab--;
    if (c_r == 0 && c_ab == 0 && c_r_b > 0)
        pthread_cond_signal (&C_r);
    pthread_mutex_unlock (&mutex);
}

void start_r(){
    pthread_mutex_lock (&mutex);
    while (c_r != 0 || c_ab != 0){
        c_r_b++;
        pthread_cond_wait (&C_r, &mutex);
        c_r_b--;
    }
    c_r++;
    pthread_mutex_unlock (&mutex);
}

void esegui_r(){
    printf ("R");
}

void end_r (){
    pthread_mutex_lock(&mutex);
    c_r--;
    if (c_r == 0 && c_ab == 0 && c_r_b > 0)
        pthread_cond_signal (&C_r);
    else if (c_r == 0 && c_ab == 0 && c_ab_b > 0)
            pthread_cond_signal (&C_ab);
    pthread_mutex_unlock (&mutex);
}

void *proc_r (void *arg){

    start_r();
    esegui_r();
    end_r();

}

void *proc_ab (void *arg){

    start_ab();
    esegui_ab();
    end_ab();

}

int main(int argc, char const *argv[])
{
    pthread_t ab, reset;

    pthread_cond_init (&C_ab, NULL);
    pthread_cond_init (&C_r, NULL);

    pthread_mutex_init (&mutex, NULL);

    srand(time(NULL));

    int t = 0;

    for (int i=0; i<10; i++){
        t = rand() % 2 + 1;
        if (t == 1)
            pthread_create (&ab, NULL, proc_ab, NULL);
        else 
            pthread_create (&reset, NULL, proc_r, NULL);

    }

    pthread_join (ab, NULL);
    pthread_join (reset, NULL);
    return 0;

}
