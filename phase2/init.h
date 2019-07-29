#ifndef INIT_H
#define INIT_H

#define FRAMESIZE 4096

void init(pcb_t *ready_queue);
void init_pcbs();
void init_areas();
void init_area(state_t* newarea, void (* handler)());

#endif
