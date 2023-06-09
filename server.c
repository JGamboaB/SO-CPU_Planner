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
int pid_count = 0;
volatile sig_atomic_t flagRun = 1;
volatile sig_atomic_t algor = 0;
char *algorithm; 
static _Atomic int TIMESF = 0;


// JobTr
typedef struct JobTr{
    int sock_fd;                // socket
} JobTr;

// Client Connection and Server connection
typedef struct Win{
    WINDOW *input;              // input window 
    WINDOW *output;             // output window
} Win;

// Client Connection and Server connection
typedef struct Connection{
    int sock_fd;                // socket server
    int new_socket;             // socket client
    struct Win *win;             // windows
} Connection;

// Send info CPU
typedef struct CPUINFO{    
    struct ReadyQueue *RQ;      // Ready Queue
    struct FinishedQueue *FQ;   // Finished Queue
    WINDOW *output;             // output window
    int rrQ;
} CPUINFO;

pthread_mutex_t cpu_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t win_mutex = PTHREAD_MUTEX_INITIALIZER;

#include "cpuscheduler.h"
ReadyQueue RQ = {NULL, NULL};
FinishQueue FQ = {NULL, NULL};

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


void *handle_connection(void *arg) {
    Connection *connection = (Connection *)arg;
    int sock_fd = connection->sock_fd;
    int new_socket = connection->new_socket;
    struct Win *win = connection->win;

    while(true) {
        Job *job = malloc(sizeof(Job));
        if (recv(sock_fd, job, sizeof(Job), 0) == -1) {
            printf("Receive failed job\n");     
            break;       
        } 
        else{
            int tempTime = TIMESF;
            while (tempTime == TIMESF){
                sleep(0.1);
            }
            
            if (flagRun == 0){
                break;
            }

            if(job->burst == 0 || job->priority == 0){
               //printf("Client desconect\n"); usar ncurses
                break;
            }

            while (stop){
                sleep(1);
            }

            pthread_mutex_lock(&win_mutex);
            char message0[100];
            for (int i = 0; i <= COLS/2; i++){
                waddstr(win->output, " ");

            }
            sprintf(message0, "[JS] PID: %d at %d\n",pid_count, TIMESF);
            waddstr(win->output, message0);
            wrefresh(win->output); 
            pthread_mutex_unlock(&win_mutex);

            //mvwprintw(win->input, 0, 0, "Command: ");   

            //printf("Received job with burst = %d, priority = %d\n", job->burst, job->priority);

            // here you need to add to the ready queue

            insert(&RQ, pid_count, job->burst, job->priority,TIMESF);
            
            //add critical section for incrementing pid like example
            pthread_mutex_lock(&cpu_mutex);
            pid_count++;
            pthread_mutex_unlock(&cpu_mutex);

            // notify client of the pid
            if (send(sock_fd, &pid_count, sizeof(pid_count), 0) < 0) {
                printf("Send failed pid\n");  
                break;       
            }
        }       
    }  

    close(sock_fd);
	pthread_detach(pthread_self());
}

void *window_thread(void *arg) {
    Win *window = (Win *)arg;
    WINDOW *input = window->input;
    WINDOW *output = window->output;  
    bool done = FALSE;  
    char bufferWin[1024];
    char buffer[2048];
    char *com =  "Command: "; 
    
    while(!done) {
        mvwprintw(input, 0, 0, com);
        if (wgetnstr(input, bufferWin, COLS - 4) != OK) {
            break;
        }

        pthread_mutex_lock(&win_mutex);
        werase(input);
        waddch(output, '\n');   /* result from wgetnstr has no newline */
        waddstr(output, "Server");
        waddstr(output, ": ");
        waddstr(output, bufferWin);
        wrefresh(output);
        pthread_mutex_unlock(&win_mutex);
        done = (*bufferWin == 4);  /* quit on control-D */

        // Here you put the commands

        if (strcmp(bufferWin, "exit") == 0) {  
            sprintf(buffer, "%s\n", bufferWin);
            flagRun = 0;
            //send(socketfd, buffer, strlen(buffer), 0);
            break;
        }
        else if (strcmp(bufferWin, "help") == 0) {
            pthread_mutex_lock(&win_mutex);
            waddstr(output, "\n\nHelp ---------------------:\n");
            waddstr(output, "\n\t'help': Display available commands");
            waddstr(output, "\n\t'stop': Stop CPU and display information");
            waddstr(output, "\n\t'queue': Consult Ready Queue");  
            waddstr(output, "\n\t'restart': Restart Program"); 
            waddstr(output, "\n\t'freeze': Freeze the screen");  
            waddstr(output, "\n\t'unfreeze': Unfreeze the screen");  
            waddstr(output, "\n\t'exit': Stop Server\n");          
            wrefresh(output);
            pthread_mutex_unlock(&win_mutex);
        }
        else if (strcmp(bufferWin, "stop") == 0) {
            //sprintf(buffer, "%s\n", bufferWin);
            stop = 1;

            pthread_mutex_lock(&win_mutex);
            werase(input);
            waddch(output, '\n');   /* result from wgetnstr has no newline */
            waddstr(output, "\nStop CPU:\n");
            char message[100];
            sprintf(message, "\n# of jobs executed: %d", RQ.finishedJobs);            
            waddstr(output, message);
            waddstr(output, "\n");
            sprintf(message, "Time in secs of idle CPU: %d", RQ.cpuOcioso);          
            waddstr(output, message);
            waddstr(output, "\n");            
            wrefresh(output);
            pthread_mutex_unlock(&win_mutex);

            if (RQ.finishedJobs != 0){
                PCB *tmp = FQ.head;
                int i = 0;
                int sumTAT = 0;
                int sumWT = 0;
                while( i != RQ.finishedJobs){
                    bzero(message, sizeof(message));
                    pthread_mutex_lock(&win_mutex);
                    sprintf(message, "Job: %d, TAT: %d, WT: %d, B: %d, P: %d, AT: %d, ET: %d", tmp->pid, tmp->turnaroundTime, tmp->waitingTime, tmp->burst, tmp->priority, tmp->startTime, tmp->endTime);          
                    waddstr(output, message);
                    waddstr(output, "\n");                
                    wrefresh(output);
                    pthread_mutex_unlock(&win_mutex);
                    usleep(500000);
                    sumTAT += tmp->turnaroundTime;
                    sumWT += tmp->waitingTime;
                    i++;
                    tmp = tmp->next;
                }
                pthread_mutex_lock(&win_mutex);
                sprintf(message, "------------------");
                waddstr(output, message);
                sprintf(message, "\nAVG. WAT: %d", sumWT/i);
                waddstr(output, message);
                waddstr(output, "\n");
                sprintf(message, "AVG. TAT: %d", sumTAT/i);
                waddstr(output, message);
                waddstr(output, "\n");
                wrefresh(output);
                pthread_mutex_unlock(&win_mutex);
            }

        } else if (strcmp(bufferWin, "queue") == 0) {
            int before = 0;
            if (stop){
                before = 1;
            }
            stop = 1;

            pthread_mutex_lock(&win_mutex);
            waddch(output, '\n');   /* result from wgetnstr has no newline */
            waddstr(output, "\nReady Queue");
            waddstr(output, ": \n");
            pthread_mutex_unlock(&win_mutex);

            char message[100];

            PCB *tmp = RQ.head;

            while( tmp != NULL){
                bzero(message, sizeof(message));
                pthread_mutex_lock(&win_mutex);
                sprintf(message, "\n\t\t\tPID: %d, Burst: %d, Priority: %d, AT: %d", tmp->pid, tmp->burst, tmp->priority, tmp->startTime);          
                waddstr(output, message);              
                wrefresh(output);
                pthread_mutex_unlock(&win_mutex);
                usleep(500000);
                tmp = tmp->next;
            }
            waddstr(output, "\n");
            wrefresh(output);

            stop = before;
        } else if (strcmp(bufferWin, "restart") == 0) {
            pthread_mutex_lock(&win_mutex);
            waddch(output, '\n');   /* result from wgetnstr has no newline */
            waddstr(output, "\nCPU Restarted\n------------------\n");
            wrefresh(output);
            pthread_mutex_unlock(&win_mutex);
            stop = 0;
        } else if (strcmp(bufferWin, "freeze") == 0) {
            pthread_mutex_lock(&win_mutex);
            waddch(output, '\n');   /* result from wgetnstr has no newline */
            wrefresh(output);
            pthread_mutex_unlock(&win_mutex);
            stop = 1;

        } else if (strcmp(bufferWin, "unfreeze") == 0) {
            pthread_mutex_lock(&win_mutex);
            waddch(output, '\n');   /* result from wgetnstr has no newline */
            wrefresh(output);
            pthread_mutex_unlock(&win_mutex);
            stop = 0;
        }else {    
            sprintf(buffer, "%s\n", bufferWin);
            // send(socketfd, buffer, strlen(buffer), 0);
        }
        // consultar ready queue
        // detener simulacion

        bzero(bufferWin, 1024);
    }
    
    flagRun = 0;
    endwin();    
    exit(EXIT_FAILURE);
	pthread_detach(pthread_self());
}   


void *start_time_thread(void *arg) {
    while(stop){
        sleep(1);
    }

    while(stop == 0){
        sleep(1);
        pthread_mutex_lock(&cpu_mutex);
        TIMESF++;
        pthread_mutex_unlock(&cpu_mutex);
    }  

    pthread_t time_id;
    pthread_create(&time_id, NULL, start_time_thread, NULL);

	pthread_detach(pthread_self());
}   

int main(int argc, char **argv) {

    if(argc < 2){
		printf("\nERROR: Missing Algorithm argument");
		printf("\nExample run: ./server FIFO\n");
        printf("\nExample run: ./server SJF\n");
        printf("\nExample run: ./server HPF\n");
        printf("\nExample run: ./server RR 2\n"); //Round Robin
		return EXIT_FAILURE;
	}

	algorithm = argv[1];    

    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    WINDOW *input, *output;
    initscr();
    cbreak();
    echo();
    input = newwin(1, COLS, LINES - 1, 0);
    output = newwin(LINES - 1, COLS, 0, 0);
    wmove(output, LINES - 2, 0);    /* start at the bottom */
    //scroll(output);
    scrollok(output, TRUE);  

    Win *window = (Win *)malloc(sizeof(Win));
    window->input = input;
    window->output = output;  

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, window_thread, (void*)window);


    pthread_t time_id;
    pthread_create(&time_id, NULL, start_time_thread, NULL);
    

    pthread_t cpu_id;
    CPUINFO *cpuinf = (CPUINFO *)malloc(sizeof(CPUINFO));
    cpuinf->RQ = &RQ;
    cpuinf->FQ = &FQ;
    cpuinf->output = output;

    char message[100];
    // cases for the different algorithms and compare the text
    if (strcmp(algorithm, "FIFO") == 0) {        
        sprintf(message, "\nServer: FIFO\n");
        waddstr(output, message);
        wrefresh(output); 
        mvwprintw(input, 0, 0, "Command: ");  
        pthread_create(&cpu_id, NULL, fifo, (void*)cpuinf);        
    } else if (strcmp(algorithm, "SJF") == 0) {        
        sprintf(message, "Server: SJF\n");
        waddstr(output, message);
        wrefresh(output); 
        mvwprintw(input, 0, 0, "Command: ");  
        pthread_create(&cpu_id, NULL, shortestJobFirst, (void*)cpuinf);     
    } else if (strcmp(algorithm, "HPF") == 0) {        
        sprintf(message, "Server: HPF\n");
        waddstr(output, message);
        wrefresh(output); 
        mvwprintw(input, 0, 0, "Command: ");  
        pthread_create(&cpu_id, NULL, highestPriorityFirst, (void*)cpuinf); 
    } else if (strcmp(algorithm, "RR") == 0) { 
        
        int rrQ = atoi(argv[2]);         
        cpuinf->rrQ = rrQ;  
        sprintf(message, "Server: RR\n");
        waddstr(output, message);
        wrefresh(output); 
        mvwprintw(input, 0, 0, "Command: ");  
        pthread_create(&cpu_id, NULL, roundRobin, (void*)cpuinf);
    } else {        
        sprintf(message, "Server: Invalid algorithm\n");
        waddstr(output, message);
        wrefresh(output); 
        mvwprintw(input, 0, 0, "Command: ");  
        return 0;
    }

    while (flagRun) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        pthread_t thread_id;
        Connection *connection = (Connection *)malloc(sizeof(Connection));
        connection->sock_fd = new_socket;
        connection->new_socket = server_fd;
        connection->win = window;
        pthread_create(&thread_id, NULL, handle_connection, (void*)connection);
    }

    shutdown(server_fd, SHUT_RDWR); // closing the listening socket

    return 0;
}



// desplegar el ready queue
