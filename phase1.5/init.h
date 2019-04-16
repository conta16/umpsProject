#ifndef INIT_H
#define INIT_H

#define FRAMESIZE 4096 //non sono molto sicuro di questa inizializzazione, conta dacci un'occhiata

#include "p1.5test_rikaya_v0.h"

pcb_t *tests[TEST_PROCS];
pcb_t ready_queue;

void init(pcb_t *ready_queue, pcb_t *tests[]);
void init_pcbs(pcb_t *tests[]);
void init_areas();
void init_area(unsigned int newarea, unsigned int handler);

#endif
