#ifndef INIT_H
#define INIT_H

#define FRAMESIZE 4096

void init(pcb_t *ready_queue, pcb_t *test_pcb);
void init_pcbs(pcb_t *test_pcb);
void init_areas();
void init_area(state_t* newarea, void (* handler)());

#endif
