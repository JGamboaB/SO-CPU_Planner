//
// Created by andre on 31/3/2023.
//

#ifndef SO_CPU_PLANNER_CPUSCHEDULER_H
#define SO_CPU_PLANNER_CPUSCHEDULER_H

// #include <unistd.h>  // For sleep
#include "jobscheduler.h"

#define TIME 1  // Used for processing simulation time


volatile sig_atomic_t stop = 0;

/*
 * @author Andres
 * @dev this function takes the first job of the ready queue and executes it until it finishes
 * @param readyQueue: a queue of structs Job to work on
 * */
void *fifo(void *arg) {
    // Unpacks the parameters
    CPUINFO *cpuinfo = (CPUINFO *)arg;
    ReadyQueue *readyQueue = cpuinfo->RQ;
    FinishQueue *FQ = cpuinfo->FQ;
    WINDOW *output = cpuinfo->output;

    // Infinite loop to always check changes in the ready queue
    while(true){
        if (stop){
            sleep(1);
        } else {
            while(true){
                //to kill infinite loops
                if(stop){
                    break;
                }

                // There are no jobs in the ready queue, simulates idle time
                while(readyQueue->head == NULL){            
                    int tempTime = TIMESF;
                    double time = 0;
                    while (tempTime == TIMESF){
                        if(readyQueue->head != NULL)
                        {
                            break;
                        }
                        sleep(0.01);
                        time += 0.01;
                    }
                    if(time >= 1){
                        readyQueue->cpuOcioso++;
                    }
                }

                // Found a job, keeps loading jobs until there are no more left
                while(readyQueue->head != NULL) {
                    if(stop){
                        break;
                    }
                    PCB* job = readyQueue->head;  // Takes the first job of the queue

                    // Prints that the job entered CPU
                    char message[100];
                    pthread_mutex_lock(&win_mutex);
                    sprintf(message, "[CPU]: PID %d start at %d\n", job->pid, TIMESF);
                    waddstr(output, message);
                    wrefresh(output); 
                    pthread_mutex_unlock(&win_mutex);

                    // Simulates the job execution time
                    int i = 0;
                    while(i < job->burst){
                        if(stop){
                            break;
                        }
                        int tempTime2 = TIMESF;
                        while (tempTime2 == TIMESF){
                            sleep(0.1);
                        }
                        i++;
                    }

                    // To kill infinite loops
                    if(stop){
                        break;
                    }

                    // The job finished, takes its statistics
                    job->endTime = TIMESF;
                    job->turnaroundTime = (job->endTime) - (job->startTime);
                    job->waitingTime = (job->turnaroundTime) - (job->burst);
                    readyQueue->finishedJobs++;

                    // Prints that the job finished
                    char message2[100];
                    pthread_mutex_lock(&win_mutex);
                    sprintf(message2, "[CPU]: PID %d finished at %d\n", job->pid, TIMESF);
                    waddstr(output, message2);
                    //mvwprintw(win->input, 0, 0, "Command: ");  
                    wrefresh(output);
                    pthread_mutex_unlock(&win_mutex);

                    delete(readyQueue,FQ, job);  // The job finished, so is removed from the queue

                    // Goes for the next job
                }
            }
        }
    }
}

/*
 * @author Andres
 * @dev this function takes the job with the shortest burst from the ready queue and executes it until it finishes
 * @param readyQueue: a queue of structs Job to work on
 * */
void shortestJobFirst(ReadyQueue *readyQueue) {
    // Unpacks the parameters
    CPUINFO *cpuinfo = (CPUINFO *)arg;
    ReadyQueue *readyQueue = cpuinfo->RQ;
    FinishQueue *FQ = cpuinfo->FQ;
    WINDOW *output = cpuinfo->output;

    // Infinite loop to always check changes in the ready queue
    while(true){
        if (stop){
            sleep(1);
        } else {
            while(true){
                //to kill infinite loops
                if(stop){
                    break;
                }

                // There are no jobs in the ready queue, simulates idle time
                while(readyQueue->head == NULL){
                    int tempTime = TIMESF;
                    double time = 0;
                    while (tempTime == TIMESF){
                        if(readyQueue->head != NULL)
                        {
                            break;
                        }
                        sleep(0.01);
                        time += 0.01;
                    }
                    if(time >= 1){
                        readyQueue->cpuOcioso++;
                    }
                }

                // Found a job, keeps loading jobs until there are no more left
                while(readyQueue->head != NULL) {
                    if(stop){
                        break;
                    }

                    PCB* job = getSJF(readyQueue); // Works with the job with the shortest burst

                    // Prints that the job entered CPU
                    char message[100];
                    pthread_mutex_lock(&win_mutex);
                    sprintf(message, "[CPU]: PID %d start at %d\n", job->pid, TIMESF);
                    waddstr(output, message);
                    wrefresh(output);
                    pthread_mutex_unlock(&win_mutex);

                    // Simulates the job execution time
                    int i = 0;
                    while(i < job->burst){
                        if(stop){
                            break;
                        }
                        int tempTime2 = TIMESF;
                        while (tempTime2 == TIMESF){
                            sleep(0.1);
                        }
                        i++;
                    }

                    // To kill infinite loops
                    if(stop){
                        break;
                    }

                    // The job finished, takes its statistics
                    job->endTime = TIMESF;
                    job->turnaroundTime = (job->endTime) - (job->startTime);
                    job->waitingTime = (job->turnaroundTime) - (job->burst);
                    readyQueue->finishedJobs++;

                    // Prints that the job finished
                    char message2[100];
                    pthread_mutex_lock(&win_mutex);
                    sprintf(message2, "[CPU]: PID %d finished at %d\n", job->pid, TIMESF);
                    waddstr(output, message2);
                    //mvwprintw(win->input, 0, 0, "Command: ");
                    wrefresh(output);
                    pthread_mutex_unlock(&win_mutex);

                    delete(readyQueue,FQ, job);  // The job finished, so is removed from the queue

                    // Goes for the next job
                }
            }
        }
    }
}

/*
 * @author Andres
 * @dev this function takes the job with the best priority from the ready queue and executes it until it finishes
 * @param readyQueue: a queue of structs Job to work on
 * */
void highestPriorityFirst(ReadyQueue *readyQueue) {
// Unpacks the parameters
    CPUINFO *cpuinfo = (CPUINFO *)arg;
    ReadyQueue *readyQueue = cpuinfo->RQ;
    FinishQueue *FQ = cpuinfo->FQ;
    WINDOW *output = cpuinfo->output;

    // Infinite loop to always check changes in the ready queue
    while(true){
        if (stop){
            sleep(1);
        } else {
            while(true){
                //to kill infinite loops
                if(stop){
                    break;
                }

                // There are no jobs in the ready queue, simulates idle time
                while(readyQueue->head == NULL){
                    int tempTime = TIMESF;
                    double time = 0;
                    while (tempTime == TIMESF){
                        if(readyQueue->head != NULL)
                        {
                            break;
                        }
                        sleep(0.01);
                        time += 0.01;
                    }
                    if(time >= 1){
                        readyQueue->cpuOcioso++;
                    }
                }

                // Found a job, keeps loading jobs until there are no more left
                while(readyQueue->head != NULL) {
                    if(stop){
                        break;
                    }

                    PCB* job = getHPF(readyQueue); // Works with the job with the best priority

                    // Prints that the job entered CPU
                    char message[100];
                    pthread_mutex_lock(&win_mutex);
                    sprintf(message, "[CPU]: PID %d start at %d\n", job->pid, TIMESF);
                    waddstr(output, message);
                    wrefresh(output);
                    pthread_mutex_unlock(&win_mutex);

                    // Simulates the job execution time
                    int i = 0;
                    while(i < job->burst){
                        if(stop){
                            break;
                        }
                        int tempTime2 = TIMESF;
                        while (tempTime2 == TIMESF){
                            sleep(0.1);
                        }
                        i++;
                    }

                    // To kill infinite loops
                    if(stop){
                        break;
                    }

                    // The job finished, takes its statistics
                    job->endTime = TIMESF;
                    job->turnaroundTime = (job->endTime) - (job->startTime);
                    job->waitingTime = (job->turnaroundTime) - (job->burst);
                    readyQueue->finishedJobs++;

                    // Prints that the job finished
                    char message2[100];
                    pthread_mutex_lock(&win_mutex);
                    sprintf(message2, "[CPU]: PID %d finished at %d\n", job->pid, TIMESF);
                    waddstr(output, message2);
                    //mvwprintw(win->input, 0, 0, "Command: ");
                    wrefresh(output);
                    pthread_mutex_unlock(&win_mutex);

                    delete(readyQueue,FQ, job);  // The job finished, so is removed from the queue

                    // Goes for the next job
                }
            }
        }
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




/*
 * UNUSED PREEMPTIVE FUNCTIONS
 * @author Andres
 * @dev this function takes the job with the shortest burst from the ready queue.
 * Since it is preemptive it executes for just 1 time unit, it does not matter if it could finish or not, and checks again
 * @param readyQueue: a queue of structs Job to work on
 * */
/*
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
}*/


/*
 * @author Andres
 * @dev this function takes the job with the best priority from the ready queue.
 * Since it is preemptive it executes for just 1 time unit, it does not matter if it could finish or not, and checks again
 * @param readyQueue: a queue of structs Job to work on
 * */
/*
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
*/



#endif //SO_CPU_PLANNER_CPUSCHEDULER_H
