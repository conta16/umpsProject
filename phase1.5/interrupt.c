#include "types_rikaya.h"
#include "listx.h"
#include "const.h"
#include "pcb.h"
#include "asl.h"
#include "utils.h"
#include "init.h"
#include "interrupt.h"
#include "syscall.h"
#include "scheduler.h"
#include "umps/libumps.h"
#include "p1.5test_rikaya_v0.h"


int rcvPLT(){
        unsigned int tmp = getCAUSE();
	tmp = tmp >> 8;
	return getBit(1,tmp);
}

int getLineInt(){
	int i;
	unsigned int tmp = getCAUSE();
	for (i=IM; i<16; i++)
		if (getBit(i,tmp)) return i;
	return -1;
}


extern void int_handler(){
	copyState((state_t *)INT_OLD_AREA, &(current->p_s));
	int line = getLineInt();
	if (line == 9){
		setTIMER((unsigned int)-1);
		scheduler(&(ready_queue.p_next));
	}
}
