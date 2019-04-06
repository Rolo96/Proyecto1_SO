/********************************************************
* Technological Institute of Costa Rica                 *
* Project Operating Systems                             *
* Teacher: Diego Vargas                                 *
* Students: Raul Arias, Bryan Abarca, Rony Paniagua     *
* Lottery scheduler                                     *
********************************************************/

#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

//Constants
#define MAXSIZE 100

//Variables
struct thread_struct_lottery {
    int tickets[MAXSIZE];
    int priority;
    pthread_mutex_t suspendMutex;
    pthread_cond_t resumeCond;
    bool suspended;
    int index;
};

struct thread_struct_lottery *queue[MAXSIZE];
int queue_size;
int raffle[MAXSIZE];
int raffle_size;
int raffle_max;

int winner;

void printRaffle(){
    int i;
    for(i=0;i<raffle_size;++i){
        printf("%d - %d\n",i,raffle[i]);
    }
}

void lottery_context_change(){
    int i;
    int j;
    if(winner!=0){
        for(i=0;i<queue_size;++i){
            for(j=0;j<queue[i]->priority;++j){
                if(queue[i]->tickets[j]==raffle[winner]){
                    queue[i]->suspended=true;
                    j=queue[i]->priority;
                    i=queue_size;
                    break;
                }
            }
        }
    }

    if(raffle_size!=0) {
        winner = rand() % (raffle_size);
        for (i = 0; i < queue_size; ++i) {
            for (j = 0; j < queue[i]->priority; ++j) {
                if (queue[i]->tickets[j] == raffle[winner]) {
                    queue[i]->suspended = false;

                    pthread_cond_signal(&(queue[i]->resumeCond));
                    j = queue[i]->priority;
                    i = queue_size;
                    break;
                }
            }
        }
    }
}

void reorderqueue(int index){
    int i;
    for(i=index;i<queue_size-1;++i){
        queue[i]=queue[i+1];
    }
    queue_size--;
}

void setruffle(int number){
    int i;
    int j;
    for(i=0;i<raffle_size;++i){
        if(raffle[i]==number){
            for(j=i;j<raffle_size-1;++j){
                raffle[j]=raffle[j+1];
            }
            i=raffle_size;
            break;
        }
    }
    raffle_size--;
}

void* lottery(void* args){
    winner=0;
    struct timeval t0;
    struct timeval t1;
    float elapsed;
    bool exec = true;
    float quantum_time = 100;
    raffle_max=0;
    raffle_size=0;

    pthread_mutex_t mutex;
    while(1){
        pthread_mutex_lock(&mutex);
        if(queue_size>0) {

            lottery_context_change();
            pthread_mutex_unlock(&mutex);
            gettimeofday(&t0, 0);
            while (exec) {
                gettimeofday(&t1, 0);
                elapsed = (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
                if (elapsed < quantum_time) {}
                else { exec = false; }
            }
            exec = true;
        }
        pthread_mutex_unlock(&mutex);
    }
}

