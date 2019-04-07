#ifndef INIT_H
#define INIT_H

void init(pcb_t *ready_queue, pcb_t tests[]);
void init_pcbs(pcb_t tests[]);
void init_areas();
void init_area(unsigned int newarea, unsigned int handler);

#endif
