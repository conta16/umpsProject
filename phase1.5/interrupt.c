#include "types_rikaya.h"
#include "const.h"
#include "utils.h"
#include "interrupt.h"
#include "scheduler.h"
#include <umps/libumps.h>

extern pcb_t* current;
extern pcb_t ready_queue;

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
