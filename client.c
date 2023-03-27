#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <limits.h>
#include "jobstruct.h" //Custom Job struct

#define PORT 8080

int create_socket() {
    int sock_fd = 0;
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // socket(domain, type, protocol)
        printf("Client creation error\n");
        exit(EXIT_FAILURE);
    }
    printf("/ / / Client started / / /\n");
    return sock_fd;
}

void init_server_address(struct sockaddr_in *serv_addr) {
    memset(serv_addr, '0', sizeof(*serv_addr));
    serv_addr->sin_family = AF_INET; // IPv4
    serv_addr->sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr->sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        exit(EXIT_FAILURE);
    }
}

void connect_to_server(int sock_fd, struct sockaddr_in serv_addr) {
    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed\n");
        exit(EXIT_FAILURE);
    }
}

void send_job(int sock_fd, Job job) {
    if (send(sock_fd, &job, sizeof(job), 0) < 0) {
        printf("Send failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Job sent - burst: %d - priority: %d\n", job.burst, job.priority);
}

void manual_mode(int sock_fd){
    char filename[25];
    char line[100];

    printf("\nEnter the number of the file: ");
    scanf("%s", filename);

    FILE* fp = fopen(filename, "r");

    if (fp == NULL) {
        perror("Error opening file");
        //exit(EXIT_FAILURE);
        return;
    }

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0'; // remove newline character
        
        if(isdigit(line[0])){
            //printf("%s\n", line);
            char b[100], *p;
            char *temp;
            strcpy(b, line);
            strtok_r(b, " ", &p);
            //printf("%s <> %s\n", b, p);

            sleep(rand() % 6 + 3); // 3 - 8 segs
            Job job = {strtol(b, &temp, 10), strtol(p, &temp, 10)};
            send_job(sock_fd, job);
        }
        
    }

    fclose(fp);
}

void auto_mode(int sock_fd){
    int n_jobs, burst;

    printf("\nEnter the number of jobs to generate: "); //n_jobs = INT_MAX;
    scanf("%d", &n_jobs);

    printf("\nEnter the maximum value of the bursts: ");
    scanf("%d", &burst);

    // Create threads to send process data to server
    for (int i = 0; i < n_jobs; i++) {
        sleep(rand() % 6 + 3); // 3 - 8 segs
        Job job = {rand() % burst + 1, rand() % 5 + 1};
        send_job(sock_fd, job);
    }
}

int main(int argc, char const *argv[]) {
    int sock_fd = create_socket();
    struct sockaddr_in serv_addr;
    init_server_address(&serv_addr);
    connect_to_server(sock_fd, serv_addr);

    //int burst, priority;

    // Choose the mode of operation
    int mode;
    printf("Choose the mode of operation:\n");
    printf("1. Manual\n");
    printf("2. Automatic\n");
    printf("Enter the mode: ");
    scanf("%d", &mode);

    switch (mode) {
        case 1:
            manual_mode(sock_fd);
            break;
        case 2:
            auto_mode(sock_fd);
            break;
        default:
            printf("Invalid mode\n");
            break;
    }

    Job end = {-1,-1};
    send_job(sock_fd, end);

    close(sock_fd);

    return 0;
}
