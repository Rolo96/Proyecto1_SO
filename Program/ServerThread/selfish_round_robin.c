/********************************************************
* Technological Institute of Costa Rica                 *
* Project Operating Systems                             *
* Teacher: Diego Vargas                                 *
* Students: Raul Arias, Bryan Abarca, Rony Paniagua     *
* Selfish Round Robin scheduler                         *
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
#define DEFAULT_TICK 50
#define DEFAULT_QUANTUM 2
#define DEFAULT_A 2
#define DEFAULT_B 1

//Variables
int a_srr;
int b_srr;
bool enqueu;
int id_control;
struct queue_thread_struct new_threads;
struct queue_thread_struct accepted_threads;

struct thread_struct_srr {
	int priority;
	bool my_turn;
	int id;
    pthread_mutex_t suspendMutex;
    pthread_cond_t resumeCond;
};

struct queue_thread_struct {
	struct thread_struct_srr *threads[MAXSIZE];
	int front;
	int rear;
	int size;
};


void displayQueue(struct queue_thread_struct queue){
    int i;
    if(queue.size>0) {
        if (queue.front <= queue.rear) {
            for (i = queue.front; i <= queue.rear; ++i) {
                printf("%d  %d\n", queue.threads[i]->id,queue.threads[i]->priority);
            }
        } else {
            for (i = queue.front; i < MAXSIZE; ++i) {
                printf("%d  %d\n", queue.threads[i]->id,queue.threads[i]->priority);
            }
            for (i = 0; i <= queue.rear; ++i) {
                printf("%d  %d\n", queue.threads[i]->id,queue.threads[i]->priority);
            }
        }
    }
}

void enqueue(struct queue_thread_struct *queue_threads,struct thread_struct_srr *thread_temp){
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

struct thread_struct_srr *dequeue(struct queue_thread_struct *queue_threads){
    if(queue_threads->size==0){
        printf("Queue is empty\n");
    }else if(queue_threads->front==queue_threads->rear){
        struct thread_struct_srr *to_return=queue_threads->threads[queue_threads->front];
        queue_threads->size--;
        queue_threads->front=0;
        queue_threads->rear=0;
        return to_return;
    }
    else{
		struct thread_struct_srr *to_return=queue_threads->threads[queue_threads->front];
        queue_threads->size--;
        if(queue_threads->front==MAXSIZE-1){
            queue_threads->front=0;
        }else{
            queue_threads->front++;
        }
		return to_return;
    }
}

/*
 * Method for: Get Quantum and Tick size from config file
 * Return: Quantum time in millisecons
 */
void getSRRConfig(float *quantum_time){
	float tick_time = DEFAULT_TICK;//In milliseconds
	float quantum = DEFAULT_QUANTUM;//In ticks
	a_srr = DEFAULT_A;
	b_srr = DEFAULT_B;
    FILE *file = fopen (CONFPATHA, "r");//Read file

    if (file != NULL)
    {
        char line[256];
        while(fgets(line, 256, file) != NULL)//Read line
        {
            if(line[0] == '#') continue;//If comment ignore
            sscanf(line, "QUANTUM = %f", &quantum);//If line is QUANTUM read it
			sscanf(line, "TICK = %f", &tick_time);//If line is TICK read it
			(*quantum_time) = quantum*tick_time;
			sscanf(line, "a = %d", &a_srr);//If line is a read it
			sscanf(line, "b = %d", &b_srr);//If line is b read it
        }
    }
}


void srr_context_change(struct queue_thread_struct *new_threads,struct queue_thread_struct *accepted_threads){
    int i;
    int j;
    for (i = 0; i < new_threads->size; ++i) {
        new_threads->threads[i]->priority+=a_srr;
    }
    for (i = 0; i < accepted_threads->size; ++i) {
        accepted_threads->threads[i]->priority+=b_srr;
    }


    int lowest_accepted_priority;
	if(accepted_threads->size>0){
		lowest_accepted_priority=accepted_threads->threads[accepted_threads->front]->priority;
		if(accepted_threads->front<accepted_threads->rear){
			for(j=accepted_threads->front;j<=accepted_threads->rear;++j){
				if(accepted_threads->threads[j]->priority<lowest_accepted_priority){
					lowest_accepted_priority=accepted_threads->threads[j]->priority;
				}
			}
		}else if(accepted_threads->front>accepted_threads->rear){
			for(j=accepted_threads->front;j<MAXSIZE;++j){
				if(accepted_threads->threads[j]->priority<lowest_accepted_priority){
					lowest_accepted_priority=accepted_threads->threads[j]->priority;
				}
			}
			for(j=0;j<accepted_threads->rear;++j){
				if(accepted_threads->threads[j]->priority<lowest_accepted_priority){
					lowest_accepted_priority=accepted_threads->threads[j]->priority;
				}
			}
		}
	}
	

	if(accepted_threads->size>0&&new_threads->size>0){
		if(new_threads->front==new_threads->rear){
            if(new_threads->threads[new_threads->front]->priority>=lowest_accepted_priority){
                struct thread_struct_srr *thread_temp;
                thread_temp=dequeue(new_threads);
                enqueue(accepted_threads,thread_temp);
            }
		}
	    else if(new_threads->front<new_threads->rear){
			for(j=new_threads->front;j<new_threads->rear;++j){
				if(new_threads->threads[j]->priority>=lowest_accepted_priority){
					struct thread_struct_srr *thread_temp;
					thread_temp=dequeue(new_threads);
					enqueue(accepted_threads,thread_temp);
				}else{break;}
			}
		}else if(new_threads->front>new_threads->rear){
			for(j=new_threads->front;j<MAXSIZE;++j){
				if(new_threads->threads[j]->priority>=lowest_accepted_priority){
					struct thread_struct_srr *thread_temp;
					thread_temp=dequeue(new_threads);
					enqueue(accepted_threads,thread_temp);
				}else{break;}
			}
			for(j=0;j<new_threads->rear;++j){
				if((*new_threads).threads[j]->priority>=lowest_accepted_priority){
					struct thread_struct_srr *thread_temp;
					thread_temp=dequeue(new_threads);
					enqueue(accepted_threads,thread_temp);
				}else{break;}
			}
		}

	}else if(accepted_threads->size<=0 && new_threads->size>0){
        struct thread_struct_srr *thread_temp;
        thread_temp=dequeue(new_threads);
        enqueue(accepted_threads,thread_temp);
	}

	if(enqueu){
        struct thread_struct_srr *thread_temp;
        thread_temp = dequeue(accepted_threads);
        enqueue(accepted_threads, thread_temp);
        enqueu=true;
	}else{
	    enqueu=true;
	}
}


void* srr(void* args){
	struct timeval t0;
   	struct timeval t1;
   	float elapsed;
	bool exec = true;
	enqueu=true;

	float quantum_time;

	id_control=0;

	new_threads.size=0;
	new_threads.front=0;
	new_threads.rear=0;
	
	accepted_threads.size=0;
	accepted_threads.front=0;
	accepted_threads.rear=0;
	//Load variables
    getSRRConfig(&quantum_time);

	quantum_time=100;
	while(1){
        if(accepted_threads.size>0) {
            gettimeofday(&t0, 0);

            accepted_threads.threads[accepted_threads.front]->my_turn = true;
            pthread_cond_signal(&accepted_threads.threads[accepted_threads.front]->resumeCond);
            while (exec) {
                gettimeofday(&t1, 0);
                elapsed = (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
                if (elapsed < quantum_time) {}
                else {
                    exec = false;
                    if(enqueu) {
                        accepted_threads.threads[accepted_threads.front]->my_turn = false;
                    }
                }
            }
            srr_context_change(&new_threads, &accepted_threads);
            exec = true;
        }
	}	
}
