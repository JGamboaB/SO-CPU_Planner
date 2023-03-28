#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "jobstruct.h" //Custom Job struct
#include <ctype.h>
#include <limits.h>

#include <ncurses.h>  //ADD AT THE END OF COMPILING THE FILE -lncurses
#include <pthread.h>
#include <signal.h>
#include <errno.h>   // for errno

#define PORT 8080


// global pid 
static _Atomic int pid_count = 0;
// JobTr
typedef struct JobTr{
    int sock_fd;                // socket
} JobTr;
pthread_mutex_t cpu_mutex = PTHREAD_MUTEX_INITIALIZER;

int create_server_socket(){
    int server_fd = 0;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { // socket(domain, type, protocol)
        perror("Server creation error");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    // Attach socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    printf("/ / / Server started / / /\n");
    return server_fd;
}


void init_server_address(struct sockaddr_in* address){
    memset(address, '0', sizeof(*address));
    address->sin_family = AF_INET; // IPv4
    address->sin_addr.s_addr = INADDR_ANY; //server will accept connections to any of the IP addresses of the server.
    address->sin_port = htons(PORT);
}

// Bind the socket to the address and port
void bind_socket_to_address(int server_fd, struct sockaddr_in address){
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("/ / Binding succesfull / /\n");
}

// Listen for incoming connections
void listen_for_incoming_connections(int server_fd, int max_queue){
    if (listen(server_fd, max_queue) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

void accept_incoming_connection(int server_fd, struct sockaddr_in address, int* new_socket, int* addrlen){
    if ((*new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
}

void *receive_job(void *arg){
    JobTr *jow = (JobTr *)arg;
    int new_socket = jow->sock_fd;
    // Receive job from client always
    while(true) {
        Job *job = malloc(sizeof(Job));
        if (recv(new_socket, job, sizeof(Job), 0) == -1) {
            printf("Receive failed job\n");            
        } 
        else{
            printf("Received job with burst = %d, priority = %d\n", job->burst, job->priority);

            // here you need to add to the ready queue

            //add critical section for incrementing pid like example
            pthread_mutex_lock(&cpu_mutex);
            pid_count++;
            pthread_mutex_unlock(&cpu_mutex);

            // notify client of the pid
            if (send(new_socket, &pid_count, sizeof(pid_count), 0) < 0) {
                printf("Send failed pid\n");
            }
        }
        

    }    
	pthread_detach(pthread_self());
}

int main(int argc, char **argv) {

    if(argc != 2){
		printf("\nERROR: Missing Algorithm argument");
		printf("\nExample run: ./server FIFO\n");
        printf("\nExample run: ./server SJF\n");
        printf("\nExample run: ./server HPF\n");
        printf("\nExample run: ./server RR\n"); //Round Robin
		return EXIT_FAILURE;
	}

	char *algorithm = argv[1];    

    int server_fd, new_socket; // Socket descriptors
    struct sockaddr_in address; // Client Address
    int addrlen = sizeof(address);

    server_fd = create_server_socket();
    init_server_address(&address);
    bind_socket_to_address(server_fd, address);
    listen_for_incoming_connections(server_fd, 3);
    accept_incoming_connection(server_fd, address, &new_socket, &addrlen);


    pthread_t jobs_thread;
    JobTr *jow = (JobTr *)malloc(sizeof(JobTr));
    jow->sock_fd = new_socket;
    if(pthread_create(&jobs_thread, NULL, &receive_job, (void*)jow) != 0){
        printf("\e[91;103;1m Error pthread send  proc\e[0m\n");
        return EXIT_FAILURE;
    }
    
    // cases for the different algorithms and compare the text
    if (strcmp(algorithm, "FIFO") == 0) {
        printf("FIFO\n");
    } else if (strcmp(algorithm, "SJF") == 0) {
        printf("SJF\n");
    } else if (strcmp(algorithm, "HPF") == 0) {
        printf("HPF\n");
    } else if (strcmp(algorithm, "RR") == 0) {
        printf("RR\n");
    } else {
        printf("Invalid algorithm\n");
    }

    while(true){
        sleep(1);
    }

    close(new_socket); // closing the connected socket
    shutdown(server_fd, SHUT_RDWR); // closing the listening socket

    return 0;
}

// desplegar el ready queue

// agreagar ncurses y reutilizar la funcion req_list del proyecto de chat leo