#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
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
    if (send(sock_fd, &job, sizeof(Job), 0) == -1) {
        printf("Send failed\n");
        exit(EXIT_FAILURE);
    }
    printf("Job sent\n");
}

int main(int argc, char const *argv[]) {
    int sock_fd = create_socket();
    struct sockaddr_in serv_addr;
    init_server_address(&serv_addr);
    connect_to_server(sock_fd, serv_addr);

    int op = 1, burst, priority;

    while(op == 1){
        printf("\n[0] Exit\t[1] Send Job: ");
        scanf("%d", &op);

        if (op!=1)
            break;

        printf("\nburst: ");
        scanf("%d", &burst);

        printf("\npriority: ");
        scanf("%d", &priority);

        Job job = {burst, priority};
        send_job(sock_fd, job);
    }

    Job end = {-1,-1};
    send_job(sock_fd, end);

    close(sock_fd);

    return 0;
}
