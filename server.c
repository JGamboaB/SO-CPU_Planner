#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "jobstruct.h" //Custom Job struct

#define PORT 8080

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

void receive_job(int new_socket, Job* job){
    if (recv(new_socket, job, sizeof(Job), 0) == -1) {
        printf("Receive failed\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char const *argv[]) {
    int server_fd, new_socket; // Socket descriptors
    struct sockaddr_in address; // Client Address
    int addrlen = sizeof(address);
    Job job; // Define instance of Job struct

    server_fd = create_server_socket();
    init_server_address(&address);
    bind_socket_to_address(server_fd, address);
    listen_for_incoming_connections(server_fd, 3);
    accept_incoming_connection(server_fd, address, &new_socket, &addrlen);
    receive_job(new_socket, &job);

    printf("Received job with burst = %d, priority = %d\n", job.burst, job.priority);

    return 0;
}
