#include <stdio.h>
#include <stdlib.h>

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

    return pcb;
}

void delete(ReadyQueue *RQ, PCB *pcb){
    if (RQ->head == pcb){ //first element
        RQ->head = pcb->next;

        if (RQ->last == pcb)//only element
            RQ->last = NULL;

        free(pcb);
        return;
    }

    PCB* tmp = RQ->head;

    while(tmp != NULL){ //search
        if (tmp->next == pcb){
            tmp->next = pcb->next;
            if (pcb->next == NULL)
                RQ->last == pcb;
            free(pcb);
            return;
        }
        tmp = tmp->next;
    }
}

PCB* getSJF(ReadyQueue *RQ){ //Get Shortest Job
    PCB* tmp = RQ->head;
    PCB* SJ = tmp;
    int shortest = tmp->burst;

    while(tmp != NULL){
        if (tmp->burst < shortest){ //Found someone shorter
            shortest = tmp->burst;
            SJ = tmp;
        } tmp = tmp->next;
    }

    return SJ;
}

PCB* getHPF(ReadyQueue *RQ){ //Get Highest Priority
    PCB* tmp = RQ->head;
    PCB* HP = tmp;
    int priority = tmp->priority;

    while(tmp != NULL){
        if (tmp->priority < priority){ //Found someone with more priority
            priority = tmp->priority;
            HP = tmp;
        } tmp = tmp->next;
    }

    return HP;
}

void printRQ(ReadyQueue *RQ){
    printf("Ready Queue:\n");
    PCB* tmp = RQ->head;

    if (tmp == NULL){
        printf("\\ \\ Empty \\ \\");
    }

    while(tmp != NULL){
        printf("-> PID: %d, Burst: %d, Priority: %d ", tmp->pid, tmp->burst, tmp->priority);
        tmp = tmp->next;
    }printf("\n");
}

int main(){
    ReadyQueue RQ = {NULL, NULL};
    PCB *a = insert(&RQ, 0, 1, 1);
    PCB *b = insert(&RQ, 1, 1, 1);
    PCB *c = insert(&RQ, 2, 1, 3);
    printRQ(&RQ);

    PCB *SJ = getSJF(&RQ);
    printf("\nShortest Job -> PID: %d, Burst: %d, Priority: %d\n", SJ->pid, SJ->burst, SJ->priority);

    PCB *HP = getHPF(&RQ);
    printf("\nHighest Priority -> PID: %d, Burst: %d, Priority: %d\n", HP->pid, HP->burst, HP->priority);


    printf("\nR1");
    ReadyQueue R1 = RQ;
    delete(&R1, a);
    printRQ(&R1);
 /*   
    printf("\nR2");
    ReadyQueue R2 = RQ;
    delete(&R2, b);
    printRQ(&R2);

    printf("\nR3");
    ReadyQueue R3 = RQ;
    delete(&R3, c);
    printRQ(&R3);

    printf("\nR4");
    ReadyQueue R4 = RQ;
    delete(&R4, a);
    delete(&R4, b);
    printRQ(&R4);

    printf("\nR5");
    ReadyQueue R5 = RQ;
    delete(&R5, a);
    delete(&R5, c);
    printRQ(&R5);

    printf("\nR6");
    ReadyQueue R6 = RQ;
    delete(&R6, b);
    delete(&R6, c);
    printRQ(&R6);*/
}