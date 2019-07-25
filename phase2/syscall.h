#ifndef SYSCALL_H
#define SYSCALL_H

#include "pcb.h"

void syscall_handler();
void syscall_error();
void terminateProcess();
void oldarea_pc_increment();
void getPids();
void getTime();
int ioCommand();
int createProcess();
void setTutor();
void kill_proc();
pcb_t* find_tutor();
void terminateProcess();


#endif
