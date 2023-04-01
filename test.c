#include <stdio.h>
#include <stdlib.h>
#include "cpuscheduler.h"
int main(){
    ReadyQueue RQ = {NULL, NULL};
    PCB *a = insert(&RQ, 0, 3, 2);
    PCB *b = insert(&RQ, 1, 4, 1);
    PCB *c = insert(&RQ, 2, 2, 3);
    PCB *d = insert(&RQ, 3, 3, 1);
    PCB *e = insert(&RQ, 4, 1, 2);
    PCB *f = insert(&RQ, 5, 2, 3);
    PCB *g = insert(&RQ, 6, 3, 1);
    PCB *h = insert(&RQ, 7, 3, 1);
    PCB *i = insert(&RQ, 8, 2, 3);

    roundRobin(&RQ, 2);

}