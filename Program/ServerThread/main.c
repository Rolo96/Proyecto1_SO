/********************************************************
* Technological Institute of Costa Rica                 *
* Project Operating Systems                             *
* Teacher: Diego Vargas                                 *
* Students: Raul Arias, Bryan Abarca, Rony Paniagua     *
* Web server thread code                                *
********************************************************/

#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<signal.h>
#include<fcntl.h>
#include <pthread.h>
#include <limits.h>
#include <time.h>
#include <stdbool.h>
#include <syslog.h>
#include <pthread.h>
#include <poll.h>
#include "lottery.c"
#include "selfish_round_robin.c"
#include "real_time.c"


//Constants
#define BYTES 1024
#define CONFPATH "/etc/WebServerThread/config.conf"
#define DEFAULT_LOG "/var/log/syslog"
#define DEFAULT_SCHEDULER "SRR"
#define MAXSIZE 100

//Variables
int listenfd;
int _port; //Port loaded from file
char* _logPath;//Log path loaded from file
char* _scheduler;
char port[6];
char root [PATH_MAX + 1] = ""; //File system root

/*****************************************************************************************
 * General methods : read config file, write logs.
*****************************************************************************************/

/*
 * Method for: Get port, logfile and scheduler type from config
 */
void getConfiguration(){
    static char path[100] = DEFAULT_LOG;//Static for just one value
    static char scheduler[100] = DEFAULT_SCHEDULER;//Static for just one value
    static int port = 8085;//Static for just one value
    FILE *file = fopen (CONFPATH, "r");//Read file

    if (file != NULL)
    {
        char line[256];
        while(fgets(line, 256, file) != NULL)//Read line
        {
            if(line[0] == '#') continue;//If comment ignore
            sscanf(line, "PORT = %d", &port);//If line is port read it
            sscanf(line, "LOGFILE = %s", path);//If line is path read it
            sscanf(line, "SCHEDULER = %s", scheduler);//If line is path read it

        }
    }
    _port=port;
    _logPath=path;
    _scheduler=scheduler;
}

/*
 * Method for: write to a file, use syslog if is system log else use write system call
 * Params message: Message to write
 *        file: Full file path
 */
void writeFile(char* message, char * file){

    char date[64];
    //Get time
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(date, sizeof(date), "%c", tm);

    int fd = open(file, O_WRONLY | O_APPEND | O_CREAT, 0775);
    if (fd >= 0) {
        write(fd, date, strlen(date));
        write(fd, message, strlen(message));

        close(fd);
    }
}

/*****************************************************************************************
 * Server methods : start methods, send response
*****************************************************************************************/

/*
 * Method for: Starts server, makes the socket, bind and listen.
 * Param port: specify port to be used
 */
void startServer(char *port)
{
    struct addrinfo hints, *res, *p;

    // getting addrinfo for host
    memset (&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo( NULL, port, &hints, &res) != 0)
    {
        writeFile("Error in method getaddrinfo \n",_logPath);
        exit(1);
    }

    //Method Socket and bind
    for (p = res; p!=NULL; p=p->ai_next)
    {
        int option = 1;
        listenfd = socket (p->ai_family, p->ai_socktype, 0);
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        if (listenfd == -1) continue;
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
    }
    if (p == NULL)
    {
        writeFile("Error in method socket or bind \n", _logPath);
        perror ("socket() or bind()");
        exit(1);
    }

    freeaddrinfo(res);

    // listen for connections
    if ( listen (listenfd, 1000000) != 0 )
    {
        writeFile("Error in method listen \n",_logPath);
        exit(1);
    }
}

void substring(char s[], char sub[], int p, int l) {
    int c = 0;

    while (c < l) {
        sub[c] = s[p+c-1];
        c++;
    }
    sub[c] = '\0';
}

/*
 * Method for: Send response to client based on the scheduler
 * Param socket_desc: indicator for client
 */
void* SendResponse(void* socket_desc)
{
    int request = (intptr_t)socket_desc;
    char *reqline[3], data_to_send[BYTES], path[BYTES];
    int fd, bytes_read;
    char mesg[BYTES], sub[BYTES];
    int i;

    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(request, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    int rcvd = recv(request, mesg, BYTES, 0);

    if (rcvd > 0) {
        struct thread_struct_lottery *thread_temp = (struct thread_struct_lottery *) malloc(sizeof(struct thread_struct_lottery));
        struct thread_struct_srr *thread_temp_srr = (struct thread_struct_srr *) malloc(sizeof(struct thread_struct_srr));
        struct thread_struct_real_time *thread_temp_real_time = (struct thread_struct_real_time *) malloc(sizeof(struct thread_struct_real_time));

        if(strncmp(_scheduler, "RT", 2) == 0){
            thread_temp_real_time->id=id_control_real_time++;
            thread_temp_real_time->my_turn=false;
            substring(mesg, sub, 6, 9);
            if (strncmp(sub, "test.png ", 9) == 0) {
                thread_temp_real_time->real_time = false;
            } else if (strncmp(sub, "test.pdf ", 9) == 0) {
                thread_temp_real_time->real_time = false;
            } else if (strncmp(sub, "test.html", 9) == 0) {
                thread_temp_real_time->real_time = false;
            } else if (strncmp(sub, "test.bin ", 9) == 0) {
                thread_temp_real_time->real_time = false;
            } else if (strncmp(sub, "test.mp4 ", 9) == 0) {
                thread_temp_real_time->real_time = true;
            }
            if(thread_temp_real_time->real_time){
                enqueue_real(&real_time,thread_temp_real_time);
            }else{
                enqueue_real(&general,thread_temp_real_time);
            }
        }else if(strncmp(_scheduler, "LOT", 3) == 0){
            thread_temp->suspended = true;
            thread_temp->index = queue_size;
            queue[queue_size] = thread_temp;
            queue_size++;
            substring(mesg, sub, 6, 9);
            if (strncmp(sub, "test.png ", 9) == 0) {
                thread_temp->priority = 4;
            } else if (strncmp(sub, "test.pdf ", 9) == 0) {
                thread_temp->priority = 6;
            } else if (strncmp(sub, "test.html", 9) == 0) {
                thread_temp->priority = 3;
            } else if (strncmp(sub, "test.bin ", 9) == 0) {
                thread_temp->priority = 6;
            } else if (strncmp(sub, "test.mp4 ", 9) == 0) {
                thread_temp->priority = 8;
            }
            pthread_mutex_t mutex;
            pthread_mutex_lock(&mutex);
            for (i = 0; i < thread_temp->priority; ++i) {
                thread_temp->tickets[i] = ++raffle_max;
                raffle[raffle_size++] = raffle_max;
            }
            pthread_mutex_unlock(&mutex);
        }else{
            thread_temp_srr->priority = a_srr;
            thread_temp_srr->my_turn = false;
            thread_temp_srr->id=id_control++;

            if(accepted_threads.size==0){
                enqueue(&accepted_threads,thread_temp_srr);
            }else{
                enqueue(&new_threads,thread_temp_srr);
            }
        }

        writeFile(mesg, _logPath);//Write message
        reqline[0] = strtok(mesg, " \t\n");
        if (strncmp(reqline[0], "GET\0", 4) == 0) {
            reqline[1] = strtok(NULL, " \t");
            reqline[2] = strtok(NULL, " \t\n");
            if (strncmp(reqline[2], "HTTP/1.0", 8) != 0 && strncmp(reqline[2], "HTTP/1.1", 8) != 0) {
                write(request, "HTTP/1.0 400 Bad Request\n", 25);
            } else {
                //if no file, open index
                if (strncmp(reqline[1], "/\0", 2) == 0) {
                    reqline[1] = "/index.html";
                }

                strcpy(path, root);
                strcpy(&path[strlen(root)], reqline[1]);

                //Write message
                char message[300] = "";
                strcat(message, "File loaded: ");
                strcat(message, path);
                strcat(message, "\n");
                writeFile(message, _logPath);

                //File found
                if ((fd = open(path, O_RDONLY)) != -1) {
                    send(request, "HTTP/1.0 200 OK\n\n", 17, 0);
                    if(strncmp(_scheduler, "RT", 2) == 0){
                        while(1) {
                            while (thread_temp_real_time->my_turn && (bytes_read = read(fd, data_to_send, BYTES)) > 0) {
                                write(request, data_to_send, bytes_read);
                            }
                            if (thread_temp_real_time->my_turn && bytes_read == 0) {
                                enqueu=false;
                                struct thread_struct_real_time *thread_tem;
                                if(thread_temp_real_time->real_time){
                                    thread_tem=dequeue_real(&real_time);
                                }else{
                                    thread_tem=dequeue_real(&general);
                                }
                                break;
                            }
                            while (!thread_temp_real_time->my_turn) {
                                pthread_cond_wait(&(thread_temp_real_time->resumeCond), &(thread_temp_real_time->suspendMutex));
                            }
                        }
                    }else if(strncmp(_scheduler, "LOT", 3) == 0){
                        while (1) {
                            while (!thread_temp->suspended && (bytes_read = read(fd, data_to_send, BYTES)) > 0) {
                                write(request, data_to_send, bytes_read);
                            }
                            if (!thread_temp->suspended && bytes_read == 0) {
                                for (i = 0; i < thread_temp->priority; ++i) {
                                    setruffle(thread_temp->tickets[i]);
                                }
                                reorderqueue(thread_temp->index);
                                break;
                            }
                            while (thread_temp->suspended) {
                                pthread_cond_wait(&(thread_temp->resumeCond), &(thread_temp->suspendMutex));
                            }
                        }
                    }else{
                        while(1) {
                            while (thread_temp_srr->my_turn && (bytes_read = read(fd, data_to_send, BYTES)) > 0) {
                                write(request, data_to_send, bytes_read);
                            }
                            if (thread_temp_srr->my_turn && bytes_read == 0) {
                                enqueu=false;
                                struct thread_struct_srr *thread_tem;
                                thread_tem=dequeue(&accepted_threads);
                                break;
                            }
                            while (!thread_temp_srr->my_turn) {
                                pthread_cond_wait(&(thread_temp_srr->resumeCond), &(thread_temp_srr->suspendMutex));
                            }
                        }
                    }
                }
                    //File not found
                else {
                    write(request, "HTTP/1.0 404 Not Found\n", 23);
                }
            }
        }
    }
    //Close socket
    shutdown (request, SHUT_RDWR);
    close(request);
}

/*****************************************************************************************
 * Main method : start and manage server
*****************************************************************************************/

/*
 * Main method, start server, accept conections and respond
 */
int main() {
    getConfiguration();
    char preRoot [PATH_MAX + 1] = "";//Stores root with server name
    int lenPreRoot = 0;//Stores full path length
    int lenServerName = 15;//Stores server name length

    //Load variables

    sprintf(port, "%d", _port);//Get port in char
    if (readlink("/proc/self/exe", preRoot, sizeof(preRoot) - 1) == -1){//Get file system root
        strcpy(preRoot,"/home/server");//If error set default
    }

    lenPreRoot = strlen(preRoot);
    strncpy( root, &preRoot[0], lenPreRoot-lenServerName);//Remove server name from path

    //Start server
    struct sockaddr_in clientaddr;
    socklen_t addrlen;
    startServer(port);

    //Message for server started
    char message[60] = "";
    strcat(message, "Web server iniciado en el puerto: ");
    strcat(message, port);
    strcat(message, ", en el directorio: ");
    strcat(message, root);
    strcat(message, "\n");
    writeFile(message, _logPath);

    pthread_t scheduler;

    if(strncmp(_scheduler, "RT", 2) == 0){
        if (pthread_create(&scheduler, NULL, rt, NULL) < 0) {
            perror("could not create thread");
            return 1;
        }
    }else if(strncmp(_scheduler, "LOT", 3) == 0){
        if (pthread_create(&scheduler, NULL, lottery, NULL) < 0) {
            perror("could not create thread");
            return 1;
        }
    }else{
        if (pthread_create(&scheduler, NULL, srr, NULL) < 0) {
            perror("could not create thread");
            return 1;
        }
    }
    pthread_t thread_id;
    //Wait for connections
    while (1)
    {
        addrlen = sizeof(clientaddr);
        int value = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);
        if (value<0){
            writeFile("Error on accept method",_logPath);}
        else{
            if( pthread_create( &thread_id , NULL ,  SendResponse , (void*) (intptr_t)value) < 0)
            {
                perror("could not create thread");
                return 1;
            }
        }
    }
    return 0;
}
