#ifndef JOBSCHEDULER_H
#define JOBSCHEDULER_H

//Envía un mensaje de confirmación al cliente con el valor del PID.
//Cada vez que un proceso termina completamente su ejecución y deja de estar en espera debe desplegarlo en pantalla.

typedef struct PCB{
    int pid;
    int burst;
    int priority;
    struct PCB* next;
} PCB;

typedef struct ReadyQueue{
    PCB* head;
    PCB* last;
} ReadyQueue;

PCB* insert(ReadyQueue *RQ, int pid, int burst, int priority){

    //Create PCB
    PCB* pcb = (PCB*)malloc(sizeof(PCB));
    pcb->pid = pid;
    pcb->burst = burst;
    pcb->priority = priority;
    pcb->next = NULL;
    pthread_mutex_lock(&cpu_mutex);
    if (RQ->head == NULL){ //empty
        RQ->head = pcb;
        RQ->last = pcb;
    } else {
        if (RQ->head->next == NULL){ //only 1 element in RQ
            RQ->head->next = pcb;
            RQ->last = pcb;
        } else {
            RQ->last->next = pcb;
            RQ->last = pcb;
        }
    }
    pthread_mutex_unlock(&cpu_mutex);
    return pcb;
}

void delete(ReadyQueue *RQ, PCB *pcb){
    pthread_mutex_lock(&cpu_mutex);
    if (RQ->head == pcb){ //first element
        
        RQ->head = pcb->next;
        if (RQ->last == pcb)//only element
            RQ->last = NULL;

        free(pcb);
        pthread_mutex_unlock(&cpu_mutex);
        return;
    }
    pthread_mutex_unlock(&cpu_mutex);

    PCB* tmp = RQ->head;

    pthread_mutex_lock(&cpu_mutex);
    while(tmp != NULL){ //search
        if (tmp->next == pcb){
            tmp->next = pcb->next;
            if (pcb->next == NULL)
                RQ->last == pcb;
            free(pcb);
            pthread_mutex_unlock(&cpu_mutex);
            return;
        }
        tmp = tmp->next;
    }
    pthread_mutex_unlock(&cpu_mutex);
}

PCB* getSJF(ReadyQueue *RQ){ //Get Shortest Job
    pthread_mutex_lock(&cpu_mutex);
    PCB* tmp = RQ->head;
    PCB* SJ = tmp;
    int shortest = tmp->burst;

    while(tmp != NULL){
        if (tmp->burst < shortest){ //Found someone shorter
            shortest = tmp->burst;
            SJ = tmp;
        } tmp = tmp->next;
    }
    pthread_mutex_unlock(&cpu_mutex);
    return SJ;
}

PCB* getHPF(ReadyQueue *RQ){ //Get Highest Priority
    pthread_mutex_lock(&cpu_mutex);
    PCB* tmp = RQ->head;
    PCB* HP = tmp;
    int priority = tmp->priority;

    while(tmp != NULL){
        if (tmp->priority < priority){ //Found someone with more priority
            priority = tmp->priority;
            HP = tmp;
        } tmp = tmp->next;
    }
    pthread_mutex_unlock(&cpu_mutex);
    return HP;
}

void printRQ(ReadyQueue *RQ){
    pthread_mutex_lock(&cpu_mutex);
    printf("Ready Queue:\n");
    PCB* tmp = RQ->head;

    if (tmp == NULL){
        printf("\\ \\ Empty \\ \\");
    }

    while(tmp != NULL){
        printf("-> PID: %d, Burst: %d, Priority: %d ", tmp->pid, tmp->burst, tmp->priority);
        tmp = tmp->next;
    }printf("\n");
    pthread_mutex_unlock(&cpu_mutex);
}

#endif