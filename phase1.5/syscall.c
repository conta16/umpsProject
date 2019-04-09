#include <umps/libumps.h>
#include "syscall.h"
#include "scheduler.h"
#include "utils.h"
#include "p1.5test_rikaya_v0.h"

unsigned int b=7;
int syscall_handler(){
	b=8;

	addokbuf("magione");
	state_t* old=(state_t*)SYSCALL_OLD_AREA;
	if (getExcCode()!=8)
		PANIC();
	addokbuf("valigia");
	switch (old->reg_a0){
	case SYS3:
		terminateProcess();
		break;
	default:
		syscall_error();
		}
	return 0;
	}

void syscall_error(){
	adderrbuf("syscallerror");
}

void terminateProcess(){
	return;
	}
