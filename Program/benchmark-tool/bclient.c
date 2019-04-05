#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <setjmp.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdbool.h>

float totalTime = 0;
int requestsCompleted, port, requestsNumber, requestsCycles, totalBytes = 0;
bool countBytes = false;
char *ip, message[1024];


int socketConnection(void *args) {
    time_t rawtime;
    struct tm timeinfo;
    time(&rawtime);
    localtime_r(&rawtime, &timeinfo);

    struct timeval requestStart, requestEnd, timeSpend;
    float i, responseAverage, responseTime = 0;
    bool count = false;
    struct sockaddr_in address;
    int sock = 0, n;
    struct sockaddr_in serv_addr;
    char buffer[1204] = {0}, textWrite[1000];
    FILE *file;

    for (i = 0; i < requestsCycles; ++i) {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("\n Socket creation error \n");
            requestsCompleted++;
            return -1;
        }

        memset(&serv_addr, '0', sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
            printf("\nInvalid address/ Address not supported \n");
            requestsCompleted++;
            return -1;
        }
        if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            printf("\nConnection Failed \n");
            requestsCompleted++;
            return -1;
        }

        if (!countBytes) {
            countBytes = true;
            count = true;
        }

        send(sock, message, strlen(message), 0);
        gettimeofday(&requestStart, NULL);
        while ((n = read(sock, buffer, 1024)) > 0) {
            if (count) totalBytes += n;
        }
        gettimeofday(&requestEnd, NULL);
        timersub(&requestEnd, &requestStart, &timeSpend);
        responseTime += (float) (timeSpend.tv_sec + ((float) timeSpend.tv_usec / 1000000));
        count = false;
    }

    responseAverage = responseTime / i;
    totalTime += responseAverage;
    sprintf(textWrite, "%d:%d:%d, %f\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, responseAverage);
    file = fopen("BenchMarkReport.csv", "a");
    fputs(textWrite, file);
    fclose(file);
    close(sock);
    requestsCompleted++;
}


int main(int argc, char *argv[]) {
    struct timeval startTime, endTime, timeSpend;
    gettimeofday(&startTime, NULL);
    time_t rawtime;

    struct tm timeinfo;
    time(&rawtime);
    localtime_r(&rawtime, &timeinfo);

    char textWrite[1000];
    float execTime;
    char *fileName, *fileExt;
    FILE *file;

    if (argc < 6) {
        printf("Too few arguments\n");
        return -1;
    } else if (argc > 6) {
        printf("Too many arguments\n");
        return -1;
    }
    printf("Executing test...\n");

    ip = argv[1];
    port = atoi(argv[2]);
    fileName = argv[3];
    sprintf(message, "GET /%s HTTP/1.0\\r\\nHost: %s:%d\\r\\n\\r\\n", fileName, ip, port);
    fileExt = strtok(fileName, ".");
    fileExt = strtok(NULL, "");
    requestsNumber = atoi(argv[4]);
    requestsCycles = atoi(argv[5]);

    sprintf(textWrite, "####################### Test begin ##################\nStartTime, AverageResponse_time\n");
    file = fopen("BenchMarkReport.csv", "a");
    fputs(textWrite, file);
    fclose(file);

    for (int i = 0; i < requestsNumber; ++i) {
        pthread_t new_thread;
        if (pthread_create(&new_thread, NULL, (void *) socketConnection, NULL) < 0) {
            perror("could not create thread");
            return -1;
        }
    }

    while (requestsCompleted < (requestsNumber)) {}
    gettimeofday(&endTime, NULL);
    timersub(&endTime, &startTime, &timeSpend);
    execTime = (float) (timeSpend.tv_sec + ((float) timeSpend.tv_usec / 1000000));

    sprintf(textWrite,
            "\nDate, Address, NumberOfRequests, FileType, FileSize, TotalExecutingTime, TotalAverageResponseTime\n%s, %s:%d,"
            " %d, .%s, %.3f KB, %f, %f\n",
            strtok(asctime(&timeinfo), "\n"), ip, port, requestsNumber, fileExt, (float) totalBytes / 1024, execTime,
            totalTime / (float) requestsNumber);
    file = fopen("BenchMarkReport.csv", "a");
    fputs(textWrite, file);
    sprintf(textWrite, "####################### Test end ##################\n\n");
    fputs(textWrite, file);
    fclose(file);
    return 0;
}
