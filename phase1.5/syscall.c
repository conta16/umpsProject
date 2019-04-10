#include <umps/libumps.h>
#include "syscall.h"
#include "scheduler.h"
#include "utils.h"
#include "p1.5test_rikaya_v0.h"
#include "init.h"

void syscall_handler(){
	state_t* old=(state_t*)SYSCALL_OLD_AREA; //renzo ha detto qualcosa del tipo di spostare il contenuto di oldarea->state in pcb->state in modo da far ripartire il processo con il processore allo stato giusto
	oldarea_pc_increment();
	if (getExcCode()!=8)
		PANIC();
	switch (old->reg_a0){
	case SYS3:
		terminateProcess();
		break;
	case 10:
		oldarea_pc_increment();
		break;
	default:
		syscall_error();
		break;
		}
	}


void syscall_error(){
	adderrbuf("syscallerror");
}

void oldarea_pc_increment(){
	state_t* old=(state_t*)SYSCALL_OLD_AREA;
	old->reg_t9+=4;
	old->pc_epc+=4;
}

void terminateProcess(){
	recursive_termination(current);
	current=NULL;
	}


void recursive_termination(pcb_t* pcb) {
	while (!emptyChild(pcb))
		recursive_termination(removeChild(pcb));
	outProcQ(&(ready_queue.p_next), pcb);
	outChild(pcb);
	freePcb(pcb);
}
