#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <limits.h>
#include "jobstruct.h" //Custom Job struct

#include <ncurses.h>  //ADD AT THE END OF COMPILING THE FILE -lncurses
#include <pthread.h>
#include <signal.h>
#include <errno.h>   // for errno

#define PORT 8080

volatile sig_atomic_t flag = 0;

// Procs
typedef struct Procs{
	int rate;            	// rate
	int maxBu; 					// max burst
    int sock_fd;                // socket
} Procs;


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

void send_job_aut(int sock_fd, Job job) {
    if (send(sock_fd, &job, sizeof(job), 0) < 0) {
        printf("Send failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Job sent - burst: %d - priority: %d\n", job.burst, job.priority);
    int pid;
    if (recv(sock_fd, &pid, sizeof(pid), 0) == -1) {
        printf("Receive failed\n");
        exit(EXIT_FAILURE);
    } 
    printf("Job recv - pid: %d \n", pid);
}


// funcion por proceso para morir de la notificacion del server



void send_job(int sock_fd, Job job) {
    //treahd
    //sleep 2 seg

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

            //define struct job
            Job job = {strtol(b, &temp, 10), strtol(p, &temp, 10)};

            //send process data to server ( job )
            send_job(sock_fd, job);
        }
        
    }

    fclose(fp);
}


void auto_mode_off(){
    // Change flag
    flag = 1;
}

// Thread function to send processes to the server in automatic mode
void *send_procs(void *arg){
    Procs *procs = (Procs *)arg;
    while (flag == 0) {
        sleep(rand() % procs->rate + 1); // 1 - rate segs
        Job job = {rand() % procs->maxBu + 1, rand() % 5 + 1};
        send_job_aut(procs->sock_fd, job);
    }
    flag = 0;    
	pthread_detach(pthread_self());
}

void auto_mode(int sock_fd){
    int rate, burst;

    printf("\nEnter the number of creation rate: ");
    scanf("%d", &rate);

    printf("\nEnter the maximum value of the bursts: ");
    scanf("%d", &burst);

    Procs * procs = (Procs *)malloc(sizeof(Procs));
    procs->rate = rate;
    procs->maxBu = burst;
    procs->sock_fd = sock_fd;
	pthread_t proc_thread;

    if(pthread_create(&proc_thread, NULL, &send_procs, (void*)procs) != 0){
		printf("\e[91;103;1m Error pthread\e[0m\n");
		exit(EXIT_FAILURE);
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
    printf("2. Automatic On\n");
    printf("3. Automatic Off\n");
    printf("Enter the mode: ");
    scanf("%d", &mode);

    switch (mode) {
        case 1:
            manual_mode(sock_fd);
            break;
        case 2:
            auto_mode(sock_fd);
            break;
        case 3:
            auto_mode_off();
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
