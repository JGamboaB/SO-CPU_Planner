#include <unistd.h>  // For sleep
#include "jobstruct.h"

#define TIME 2  // Used for processing simulation time

/*
 * @author Andres
 * @dev this function takes the first job of the ready queue and executes it until it finishes
 * @param readyQueue: a queue of structs Job to work on
 * */
void fifo(struct ReadyQueue readyQueue) {
    // Keeps loading jobs until there are no more left
    while(!readyQueue.isEmpty()) {
        struct Job job = readyQueue.pop(0);  // Takes the first job of the queue, pops it to also removed from the queue

        // Simulates the burst of the process
        while(job.burst > 0){
            sleep(TIME);  // Simulates it has taken 1 time unit
            job.burst--;  // Since it has advanced, the process is 1 unit closer to end so its burst has to decrease
        }
        // Goes for the next job
    }
}

/*
 * @author Andres
 * @dev this function takes the job with the shortest burst from the ready queue and executes it until it finishes
 * @param readyQueue: a queue of structs Job to work on
 * */
void shortestJobFirst(struct ReadyQueue readyQueue) {
    // Keeps loading jobs until there are no more left
    while(!readyQueue.isEmpty()) {
        // First it searches for the shortest burst
        int minBurst = 100;  // This variable will be used to compare which job from all has the shortest burst
        int shortestJobIndex = 0;  // This tells the index of  the job we will chose, to later access it
        // Goes job by job checking if we can get a shortest burst
        for(int i = 0; i < readyQueue.length(); i++) {
            int currentBurst = readyQueue.get(i).burst;

            // Checks if the current job has a shortest burst than the shortest we have found so far
            if(currentBurst < minBurst) {
                shortestJobIndex = i;  // Updates the position of the job with the shortest burst of all
                minBurst = currentBurst;  // Updates to the even shorter burst we just found

                if(minBurst == 1){
                    break;  // There cannot be a shortest burst than 1, with 1 we reached the best so there is no point of continue searching
                }
            }
        }

        // Works with the job with the shortest burst
        struct Job job = readyQueue.get(shortestJobIndex);  // Takes the shortest job of the queue, pops it to also removed from the queue
        // Simulates the burst of the process
        while(job.burst > 0){
            sleep(TIME);  // Simulates it has taken 1 time unit
            job.burst--;  // Since it has advanced, the process is 1 unit closer to end so its burst has to decrease
        }
        // Goes for the next job
    }
}

/*
 * @author Andres
 * @dev this function takes the job with the shortest burst from the ready queue.
 * Since it is preemptive it executes for just 1 time unit, it does not matter if it could finish or not, and checks again
 * @param readyQueue: a queue of structs Job to work on
 * */
void shortestJobFirstPreemptive(struct ReadyQueue readyQueue) {
    // Keeps loading jobs until there are no more left
    while(!readyQueue.isEmpty()) {
        // First it searches for the shortest burst
        int minBurst = 100;  // This variable will be used to compare which job from all has the shortest burst
        int shortestJobIndex = 0;  // This tells the index of  the job we will chose, to later access it
        // Goes job by job checking if we can get a shortest burst
        for(int i = 0; i < readyQueue.length(); i++) {
            int currentBurst = readyQueue.get(i).burst;

            // Checks if the current job has a shortest burst than the shortest we have found so far
            if(currentBurst < minBurst) {
                shortestJobIndex = i;  // Updates the position of the job with the shortest burst of all
                minBurst = currentBurst;  // Updates to the even shorter burst we just found

                if(minBurst == 1){
                    break;  // There cannot be a shortest burst than 1, with 1 we reached the best so there is no point of continue searching
                }
            }
        }

        // Works with the job with the shortest burst
        struct Job job = readyQueue.get(shortestJobIndex);  // Takes the shortest job of the queue, pops it to also removed from the queue

        // We cannot execute all the burst of the selected process, we must go 1 by 1 to check if a shortest one came
        sleep(TIME);  // Simulates it has taken 1 time unit
        job.burst--;  // Since it has advanced, the process is 1 unit closer to end so its burst has to decrease

        // Checks if the job ended or still has burst to execute
        if(job.burst > 0){
            readyQueue.push(job);  // Puts it back at the end of the queue to wait for a chance to continue with its unfinished execution, remember at the we popped it so we removed form the queue
        }

        // Goes for the next job
    }
}

/*
 * @author Andres
 * @dev this function takes the job with the best priority from the ready queue and executes it until it finishes
 * @param readyQueue: a queue of structs Job to work on
 * */
void highestPriorityFirst(struct ReadyQueue readyQueue) {
    // Keeps loading jobs until there are no more left
    while(!readyQueue.isEmpty()) {
        // First it searches for the best priority
        int bestPriority = 100;  // This variable will be used to compare which from all job has the best priority
        int mostImportantJobIndex = 0;  // This tells the index of  the job we will chose, to later access it
        // Goes job by job checking if we can get a better priority
        for(int i = 0; i < readyQueue.length(); i++) {
            int currentPriority = readyQueue.get(i).priority;

            // Checks if the current job has a better priority than the best we have found so far
            if(currentPriority < bestPriority) {
                mostImportantJobIndex = i;  // Updates the position of the job with the shortest burst of all
                bestPriority = currentPriority;  // Updates to the even shorter burst we just found

                if(bestPriority == 1){
                    break;  // There cannot be a better priority than 1, with 1 we reached the best so there is no point of continue searching
                }
            }
        }

        // Works with the job with the bestPriority
        struct Job job = readyQueue.get(mostImportantJobIndex);  // Takes the shortest job of the queue, pops it to also removed from the queue
        // Simulates the burst of the process
        while(job.burst > 0){
            sleep(TIME);  // Simulates it has taken 1 time unit
            job.burst--;  // Since it has advanced, the process is 1 unit closer to end so its burst has to decrease
        }

        // Goes for the next job
    }
}

/*
 * @author Andres
 * @dev this function takes the job with the best priority from the ready queue.
 * Since it is preemptive it executes for just 1 time unit, it does not matter if it could finish or not, and checks again
 * @param readyQueue: a queue of structs Job to work on
 * */
void highestPriorityPreemptive(struct ReadyQueue readyQueue) {
    // Keeps loading jobs until there are no more left
    while(!readyQueue.isEmpty()) {
        // First it searches for the best priority
        int bestPriority = 100;  // This variable will be used to compare which from all job has the best priority
        int mostImportantJobIndex = 0;  // This tells the index of  the job we will chose, to later access it
        // Goes job by job checking if we can get a better priority
        for(int i = 0; i < readyQueue.length(); i++) {
            int currentPriority = readyQueue.get(i).priority;

            // Checks if the current job has a better priority than the best we have found so far
            if(currentPriority < bestPriority) {
                mostImportantJobIndex = i;  // Updates the position of the job with the shortest burst of all
                bestPriority = currentPriority;  // Updates to the even shorter burst we just found

                if(bestPriority == 1){
                    break;  // There cannot be a better priority than 1, with 1 we reached the best so there is no point of continue searching
                }
            }
        }

        // Works with the job with the bestPriority
        struct Job job = readyQueue.get(mostImportantJobIndex);  // Takes the shortest job of the queue, pops it to also removed from the queue

        // We cannot execute all the burst of the selected process, we must go 1 by 1 to check if one with a highest priority came
        sleep(TIME);  // Simulates it has taken 1 time unit
        job.burst--;  // Since it has advanced, the process is 1 unit closer to end so its burst has to decrease

        // Checks if the job ended or still has burst to execute
        if(job.burst > 0){
            readyQueue.push(job);  // Puts it back at the end of the queue to wait for a chance to continue with its unfinished execution, remember at the we popped it so we removed form the queue
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
void roundRobin(struct ReadyQueue readyQueue, int q) {
    // Keeps loading jobs until there are no more left
    while (!readyQueue.isEmpty()) {
        struct Job job = readyQueue.pop(0);  // Takes the first job of the queue

        char finished = 0;  // This works as a flag that tells if the job finished in that quantum or needs to wait for the next one
        // Simulates the burst of the quantum
        for (int i = 0; i < q; i++) {
            sleep(TIME);  // Simulates it has taken 1 time unit
            job.burst--;  // Since it has advanced, the process is 1 unit closer to end so its burst has to decrease

            // Checks if the process ended
            if (job.burst == 0) {
                finished = 1;  // Updates the flag
                break;  // Since the job finished, there is no point to continue with this quantum
            }
        }

        // Since we popped the job in the first line, it is no longer in the queue, however it may needs to return to the queue because it did not finish
        if(!finished){
            readyQueue.push(job);  // Puts it back at the end of the queue to wait for a chance to continue with its unfinished execution
        }

        // Goes for the next job
    }
}