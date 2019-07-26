#ifndef SYSCALL_H
#define SYSCALL_H

#include "pcb.h"

void syscall_handler();
void syscall_error();
void oldarea_pc_increment();
void getPids();
void getTime();
unsigned int ioCommand();
int createProcess();
void setTutor();
void kill_proc();
pcb_t* find_tutor();
int terminateProcess(void **pid);
void verhogen();
void passeren();
int specpassup();


#endif
