#ifndef SYSCALL_H
#define SYSCALL_H

#include "pcb.h"

void syscall_handler();
void syscall_error();
void sys_return(state_t* old);
void oldarea_pc_increment();
void get_pids();
void get_time();
void io_command();
int create_process();
void set_tutor();
void kill_proc();
pcb_t* find_tutor();
int terminate_process(void **pid);
void verhogen();
void passeren();
void wait_clock();
int spec_passup();


#endif
