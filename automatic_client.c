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


void *send_job_aut(void *arg){
    Procs *procs = (Procs *)arg;
    Job job = {rand() % procs->maxBu + 1, rand() % 5 + 1};

    if (send(procs->sock_fd, &job, sizeof(job), 0) < 0) {
        printf("Send proc failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Job sent - burst: %d - priority: %d\n", job.burst, job.priority);
    int pid;
    if (recv(procs->sock_fd, &pid, sizeof(pid), 0) == -1) {
        printf("Receive failed\n");
        exit(EXIT_FAILURE);
    } 
    printf("Job recv - pid: %d \n", pid);
	pthread_detach(pthread_self());
}

int main(int argc, char const *argv[]) {
    int sock_fd = create_socket();
    struct sockaddr_in serv_addr;
    init_server_address(&serv_addr);
    connect_to_server(sock_fd, serv_addr);

   int rate, burst;

    printf("\nEnter the number of creation rate: ");
    scanf("%d", &rate);

    printf("\nEnter the maximum value of the bursts: ");
    scanf("%d", &burst);

    Procs * procs = (Procs *)malloc(sizeof(Procs));
    procs->rate = rate;
    procs->maxBu = burst;
    procs->sock_fd = sock_fd;

    while (true) {       
        sleep(rand() % procs->rate + 1); // 1 - rate segs
        pthread_t proc_thread;
        if(pthread_create(&proc_thread, NULL, &send_job_aut, (void*)procs) != 0){
            printf("\e[91;103;1m Error pthread send  proc\e[0m\n");
            return EXIT_FAILURE;
        }
    } 

    close(sock_fd);

    return 0;
}
