//
// Created by andre on 31/3/2023.
//

#ifndef SO_CPU_PLANNER_CPUSCHEDULER_H
#define SO_CPU_PLANNER_CPUSCHEDULER_H

// #include <unistd.h>  // For sleep
#include "jobscheduler.h"

#define TIME 1  // Used for processing simulation time


volatile sig_atomic_t stop = 0;

void endJob(ReadyQueue *readyQueue, PCB *pcb){
    //printf("\nTerminado proceso %d", pcb->pid);
    TIMESF++;
    delete(readyQueue, pcb);  // The job finished, so is removed from the queue delete(readyQueue, job);  // The job finished, so is removed from the queue
}


/*
 * @author Andres
 * @dev this function takes the first job of the ready queue and executes it until it finishes
 * @param readyQueue: a queue of structs Job to work on
 * */
void *fifo(void *arg) {
    CPUINFO *cpuinfo = (CPUINFO *)arg;.
    ReadyQueue *readyQueue = cpuinfo->RQ;
    WINDOW *output = cpuinfo->output;

    //agregar atributo ReadyQueue cpuOcioso
    while(true){
        if(stop){
            break;
        }

        while(readyQueue->head == NULL){
            readyQueue->cpuOcioso++;
            sleep(TIME);
        }

        // Keeps loading jobs until there are no more left
        while(readyQueue->head != NULL) {
            if(stop){
                break;
            }
            PCB* job = readyQueue->head;  // Takes the first job of the queue

            // How to print to the window
            char message[100];
            sprintf(message, "\nFIFO [server]: Proceso %d con burst %d y prioridad %d entra en ejecución.", job->pid, job->burst, job->priority);
            waddstr(output, message);
            wrefresh(output); 
            
            // Simulates the burst of the process
            sleep(job->burst);

            /*
            // Simulates the burst of the process
            while(job->burst > 0){
                sleep(ob->burst);  // Simulates it has taken 1 time unit
                job->burst--;  // Since it has advanced, the process is 1 unit closer to end so its burst has to decrease
            }*/

            sprintf(message, "\nFIFO [server]: Proceso %d terminó.", job->pid);
            waddstr(output, message);
            //mvwprintw(win->input, 0, 0, "Command: ");  
            wrefresh(output);

            endJob(readyQueue, job);
            // Goes for the next job
        }
    }
}

/*
 * @author Andres
 * @dev this function takes the job with the shortest burst from the ready queue and executes it until it finishes
 * @param readyQueue: a queue of structs Job to work on
 * */
void shortestJobFirst(ReadyQueue *readyQueue) {
    // Keeps loading jobs until there are no more left
    while(readyQueue->head != NULL) {
        PCB* job = getSJF(readyQueue); // Works with the job with the shortest burst

        // Simulates the burst of the process
        while(job->burst > 0){
            printf("\nSe ejecuto por 1 el proceso %d", job->pid);
            sleep(TIME);  // Simulates it has taken 1 time unit
            job->burst--;  // Since it has advanced, the process is 1 unit closer to end so its burst has to decrease
        }
        endJob(readyQueue, job);
        // Goes for the next job
    }
}

/*
 * @author Andres
 * @dev this function takes the job with the shortest burst from the ready queue.
 * Since it is preemptive it executes for just 1 time unit, it does not matter if it could finish or not, and checks again
 * @param readyQueue: a queue of structs Job to work on
 * */
void shortestJobFirstPreemptive(ReadyQueue *readyQueue) {
    // Keeps loading jobs until there are no more left
    while(readyQueue->head != NULL) {
        PCB* job = getSJF(readyQueue); // Works with the job with the shortest burst

        // We cannot execute all the burst, we must go 1 by 1 to check if one with a highest priority came
        printf("\nSe ejecuto por 1 el proceso %d", job->pid);
        sleep(TIME);  // Simulates it has taken 1 time unit
        job->burst--;  // Since it has advanced, the process is 1 unit closer to end so its burst has to decrease

        // Checks if the job ended or still has burst to execute
        if(job->burst == 0){
            endJob(readyQueue, job);
        }

        // Goes for the next job
    }
}

/*
 * @author Andres
 * @dev this function takes the job with the best priority from the ready queue and executes it until it finishes
 * @param readyQueue: a queue of structs Job to work on
 * */
void highestPriorityFirst(ReadyQueue *readyQueue) {
// Keeps loading jobs until there are no more left
    while(readyQueue->head != NULL) {
        PCB* job = getHPF(readyQueue); // Works with the job with the best priority

        // Simulates the burst of the process
        while(job->burst > 0){
            printf("\nSe ejecuto por 1 el proceso %d", job->pid);
            sleep(TIME);  // Simulates it has taken 1 time unit
            job->burst--;  // Since it has advanced, the process is 1 unit closer to end so its burst has to decrease
        }
        endJob(readyQueue, job);
        // Goes for the next job
    }}

/*
 * @author Andres
 * @dev this function takes the job with the best priority from the ready queue.
 * Since it is preemptive it executes for just 1 time unit, it does not matter if it could finish or not, and checks again
 * @param readyQueue: a queue of structs Job to work on
 * */
void highestPriorityPreemptive(ReadyQueue *readyQueue) {
    // Keeps loading jobs until there are no more left
    while(readyQueue->head != NULL) {
        PCB* job = getHPF(readyQueue); // Works with the job with the highest priority

        // We cannot execute all the burst, we must go 1 by 1 to check if one with a highest priority came
        printf("\nSe ejecuto por 1 el proceso %d", job->pid);
        sleep(TIME);  // Simulates it has taken 1 time unit
        job->burst--;  // Since it has advanced, the process is 1 unit closer to end so its burst has to decrease

        // Checks if the job ended or still has burst to execute
        if(job->burst == 0){
            endJob(readyQueue, job);
        }

        // Goes for the next job
    }
}


/*
 * @author Andres
 * @dev this function takes the first job from the ready queue and executes in quantum of time.
 * If the process finishes in the quantum everything is ok, if it finishes before the quantum ends, the quantum is just cut earlier and goes with the next quantum
 * If the process could not finish in the quantum, bad luck, it goes to the end of the queue and has to wait for its next chance to continue executing
 * @param readyQueue: a queue of structs Job to work on
 * @param q, tells the length of the quantum
 * */
void roundRobin(ReadyQueue *readyQueue, int q) {
    // Keeps loading jobs until there are no more left
    while(readyQueue->head != NULL) {
        PCB* job = readyQueue->head;  // Takes the first job of the queue

        char finished = 0;  // This works as a flag that tells if the job finished in that quantum or needs to wait for the next one
        // Simulates the burst of the quantum
        for (int i = 0; i < q; i++) {
            printf("\nSe ejecuto por 1 el proceso %d", job->pid);
            sleep(TIME);  // Simulates it has taken 1 time unit
            job->burst--;  // Since it has advanced, the process is 1 unit closer to end so its burst has to decrease

            // Checks if the process ended
            if (job->burst == 0) {
                delete(readyQueue, job);  // Takes the job out of the queue
                finished = 1;  // Updates the flag
                break;  // Since the job finished, there is no point to continue with this quantum
            }
        }

        if (!finished){
            // Moves the job from head to last
            moveFirstToLast(readyQueue);
        }

        // Goes for the next job
    }
}



#endif //SO_CPU_PLANNER_CPUSCHEDULER_H
