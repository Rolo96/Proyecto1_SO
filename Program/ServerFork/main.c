/********************************************************
* Instituto Tecnologico de Costa Rica                   *
* Proyecto Sistemas operativos                          *
* Profesor: Diego Vargas                                *
* Estudiantes: Raul Arias, Bryan Abarca, Rony Paniagua  *
* Web server fork                                       *
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
#include <limits.h>
#include <time.h>
#include <stdbool.h>
#include <syslog.h>

//Constants
#define BYTES 1024
#define CONFPATH "/etc/WebServerFork/config.conf"
#define DEFAULT_LOG "/var/log/syslog"

//Variables
int listenfd;
int _port; //Port loaded from file
char* _logPath;//Log path loaded from file
char port[6];
char *root;

/*****************************************************************************************
 * General methods : read config file, write logs.
*****************************************************************************************/

/*
 * Method for: Get port from config file
 * Return: If doesnt find the port set 8083
 */
int getPort(){

    static int port = 8083;//Static for just one value
    FILE *file = fopen (CONFPATH, "r");//Read file

    if (file != NULL)
    {
        char line[256];
        while(fgets(line, 256, file) != NULL)//Read line
        {
            if(line[0] == '#') continue;//If comment ignore
            sscanf(line, "PORT = %d", &port);//If line is port read it
        }
    }
    return port;
}

/*
 * Method for: Get path from config file
 * Return: If doesnt find the path set /var/log/syslog
 */
char* getLogPath(){

    static char path[100] = DEFAULT_LOG;//Static for just one value
    FILE *file = fopen (CONFPATH, "r");//Read file

    if (file != NULL)
    {
        char line[256];
        while(fgets(line, 256, file) != NULL)//Read line
        {
            if(line[0] == '#') continue;//If comment ignore
            sscanf(line, "LOGFILE = %s", path);//If line is path read it
        }
    }

    return path;
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

/*
 * Method for: Send response to client
 * Param request: indicator for client
 */
void SendResponse(int request)
{
    char mesg[BYTES];
    char *reqline[3], data_to_send[BYTES], path[BYTES];
    int fd, bytes_read;

    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(request, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    int rcvd = recv(request, mesg, BYTES, 0);
    if (rcvd > 0) {
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
                    while ((bytes_read = read(fd, data_to_send, BYTES)) > 0)
                        write(request, data_to_send, bytes_read);
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
    char preRoot [PATH_MAX + 1] = "";//Stores root with server name
    int lenPreRoot = 0;//Stores full path length
    int lenServerName = 13;//Stores server name length

    //Load variables
    _port = getPort();
    _logPath = getLogPath();
    sprintf(port, "%d", _port);//Get port in char
    root = getenv("PWD");

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
    printf("%s\n",port);
    //Wait for connections
    while (1)
    {
        addrlen = sizeof(clientaddr);
        int value = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);
        if (value<0){
            writeFile("Error on accept method",_logPath);}
        else
        {
            int pid = fork();
            if ( pid==0 )
            {
                SendResponse(value);
                exit(0);
            }
        }
        signal(SIGCHLD,SIG_IGN);
    }
    return 0;
}
