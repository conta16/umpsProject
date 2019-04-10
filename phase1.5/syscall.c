#include <umps/libumps.h>
#include "syscall.h"
#include "scheduler.h"
#include "utils.h"
#include "p1.5test_rikaya_v0.h"

void syscall_handler(){
	state_t* old=(state_t*)SYSCALL_OLD_AREA;
	if (getExcCode()!=8)
		PANIC();
	switch (old->reg_a0){
	case SYS3:
		terminateProcess();
		break;
/*	default:
		syscall_error();*/
		}
	}


void syscall_error(){
	adderrbuf("syscallerror");
}


void terminateProcess(){
	addokbuf("chiamata di sys3");
	}
