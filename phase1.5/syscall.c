#include "syscall.h"
#include "scheduler.h"
#include "p1.5test_rikaya_v0.c"
int syscall_handler(){

	if (getExcCode!=8)
		PANIC();
	switch (*reg_a0){
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
