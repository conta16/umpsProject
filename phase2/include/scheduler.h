#ifndef SCHED_H
#define SCHED_H

#include "listx.h"

void scheduler(struct list_head* head);
void scheduler_init(struct list_head* head);
void increment_pcbs_priority(struct list_head* head);

#endif
