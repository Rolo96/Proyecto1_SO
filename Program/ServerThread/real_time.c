/********************************************************
* Technological Institute of Costa Rica                 *
* Project Operating Systems                             *
* Teacher: Diego Vargas                                 *
* Students: Raul Arias, Bryan Abarca, Rony Paniagua     *
* Real time scheduler                                   *
********************************************************/

#include <sys/time.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

//Constants
#define CONFPATHA "/etc/WebServerThread/config.conf"
#define MAXSIZE 100

bool enqueu;
bool real_running;
int id_control_real_time;
int id_running;
struct queue_thread_struct_real real_time;
struct queue_thread_struct_real general;
struct thread_struct_real_time {
    bool real_time;
    bool my_turn;
    int id;
    pthread_mutex_t suspendMutex;
    pthread_cond_t resumeCond;
};

struct queue_thread_struct_real {
    struct thread_struct_real_time *threads[MAXSIZE];
    int front;
    int rear;
    int size;
};

void displayQueue_real(struct queue_thread_struct_real queue){
    int i;
    if(queue.size>0) {
        if (queue.front <= queue.rear) {
            for (i = queue.front; i <= queue.rear; ++i) {
                printf("%d \n", queue.threads[i]->id);
            }
        } else {
            for (i = queue.front; i < MAXSIZE; ++i) {
                printf("%d \n", queue.threads[i]->id);
            }
            for (i = 0; i <= queue.rear; ++i) {
                printf("%d \n", queue.threads[i]->id);
            }
        }
    }
}

void enqueue_real(struct queue_thread_struct_real *queue_threads,struct thread_struct_real_time *thread_temp){
    if((*queue_threads).size<MAXSIZE){
        if(queue_threads->size==0){
            queue_threads->threads[0] = thread_temp;
            queue_threads->front = queue_threads->rear = 0;
            queue_threads->size = 1;
        }
        else if(queue_threads->rear == MAXSIZE-1){
            queue_threads->threads[0] = thread_temp;
            queue_threads->rear = 0;
            queue_threads->size++;
        }
        else{
            queue_threads->threads[queue_threads->rear+1] = thread_temp;
            queue_threads->rear++;
            queue_threads->size++;
        }
    }
    else{
        printf("Queue is full\n");
    }
}

struct thread_struct_real_time *dequeue_real(struct queue_thread_struct_real *queue_threads){
    if(queue_threads->size==0){
        printf("Queue is empty\n");
    }else if(queue_threads->front==queue_threads->rear){
        struct thread_struct_real_time *to_return=queue_threads->threads[queue_threads->front];
        queue_threads->size--;
        queue_threads->front=0;
        queue_threads->rear=0;
        return to_return;
    }
    else{
        struct thread_struct_real_time *to_return=queue_threads->threads[queue_threads->front];
        queue_threads->size--;
        queue_threads->front++;
        return to_return;
    }
}

void real_time_context_change(){
    if(enqueu){
        if(real_running){
            struct thread_struct_real_time *thread_temp;
            thread_temp = dequeue_real(&real_time);
            enqueue_real(&real_time, thread_temp);
        }else{
            struct thread_struct_real_time *thread_temp;
            thread_temp = dequeue_real(&general);
            enqueue_real(&general, thread_temp);
        }
        enqueu=true;
    }else{
        enqueu=true;
    }
}

void* rt(void* args){
    struct timeval t0;
    struct timeval t1;
    float elapsed;
    bool exec = true;
    enqueu=true;

    float quantum_time;

    id_control_real_time=0;

    real_time.size=0;
    real_time.front=0;
    real_time.rear=0;

    general.size=0;
    general.front=0;
    general.rear=0;

    quantum_time=100;

    while(1){
        if(real_time.size>0||general.size>0) {
            gettimeofday(&t0, 0);
            if(real_time.size>0){
                real_time.threads[real_time.front]->my_turn = true;
                pthread_cond_signal(&real_time.threads[real_time.front]->resumeCond);
                real_running=true;
            }else{
                general.threads[general.front]->my_turn = true;
                pthread_cond_signal(&general.threads[general.front]->resumeCond);
                real_running=false;
            }
            while (exec) {
                gettimeofday(&t1, 0);
                elapsed = (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
                if (elapsed < quantum_time) {}
                else {
                    exec = false;
                    if(enqueu) {
                        if(real_running){
                            real_time.threads[real_time.front]->my_turn = false;
                        }else{
                            general.threads[general.front]->my_turn = false;
                        }
                    }
                }
            }
            real_time_context_change(&real_time, &general);
            exec = true;
        }
    }
}
