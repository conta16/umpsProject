#ifndef SCHED_H
#define SCHED_H

#include "pcb.h"
#include "p1.5test_rikaya_v0.h"

pcb_t* current;
struct OrPr {
	pcb_t* proc;
	unsigned int original;
};

struct OrPr OrVec[TEST_PROCS];

void scheduler();
void scheduler_init();
void increment_pcbs_priority();

#endif
