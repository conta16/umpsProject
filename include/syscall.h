#ifndef SYSCALL_H
#define SYSCALL_H

#include "pcb.h"

void syscall_handler();
void syscall_error();
void sys_return(state_t* old);
void oldarea_pc_increment();
void get_pids(void ** pid, void ** ppid);
void get_time (unsigned int *user, unsigned int *kernel, unsigned int *wallclock);
void io_command(unsigned int command, unsigned int *ourReg, int type);
void create_process(state_t *statep, int priority, void ** cpid);
void set_tutor();
void kill_proc(pcb_t* pid);
pcb_t* find_tutor(pcb_t* pid);
int terminate_process(void **pid);
void verhogen(int* semaddr);
void passeren(int* semaddr, state_t* block_state);
void wait_clock();
int spec_passup(int type, state_t* old, state_t* new);


#endif
