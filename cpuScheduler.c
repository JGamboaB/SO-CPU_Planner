#include "jobstruct.h"

void fifo() {
    while(!colaProcesos.isEmpty()) {
        struct Job job = colaProcesos.pop(0);

        while(job.burst > 0){
            thread.sleep(1000);
            job.burst--;
        }
    }
}

void shortestJobFirst() {
    while(!colaProcesos.isEmpty()) {
        struct Job job;

        int burst = 100;

        int i = 0;
        while (colaProcesos.get(i).next != null;) {
            if(colaProcesos.get(i).burst < burst) {
                job = colaProcesos.get(i);
                burst = job.burst;
                break;
            }

            i++;
        }

        while(job.burst > 0){
            thread.sleep(1000);
            job.burst--;
        }

        colaProcesos.pop(i);
    }

}

void shortestJobFirstPreemptive() {


}

void highestPriorityFirst() {
    while(!colaProcesos.isEmpty()) {
        struct Job job;

        int priority = 100;

        int i = 0;
        while (colaProcesos.get(i).next != null;) {
            if(colaProcesos.get(i).priority < priority) {
                job = colaProcesos.get(i);
                burst = job.priority;
                break;
            }

            i++;
        }

        while(job.burst > 0){
            thread.sleep(1000);
            job.burst--;
        }

        colaProcesos.pop(i);
    }

}

void highestPriorityPreemptive() {

}

void roundRobin(int q) {
    while(!colaProcesos.isEmpty()) {
        struct Job job = colaProcesos.get(0);

        char flag = 0;
        for(int i = 0; i < q; i++){
            thread.sleep(1000);
            job.burst--;

            if (job.burst == 0){
                colaProcesos.pop(0);
                flag = 1;
                break;
            }
        }

        if(flag){
            colaProcesos.pop(0);
            colaProcesos.push(job);
        }
    }

}