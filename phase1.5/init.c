#include "types_rikaya.h"
#include "listx.h"
#include "const.h"
#include "pcb.h"
#include "asl.h"
#include "utils.h"
#include "init.h"
#include "syscall.h"
#include "p1.5test_rikaya_v0.c"

void init_area(unsigned int newarea, unsigned int handler){
	state_t *tmp = newarea;
	tmp->pc_epc = handler;
	tmp->reg_sp = RAMTOP;
	setBit(IEc,&(tmp->status),0);
	setBit(VMc,&(tmp->status),0);
	setBit(KUc,&(tmp->status),0);
	setBit(TE,&(tmp->status),1);
}

void init_areas(){
	init_area(INT_NEW_AREA, int_handler);
	init_area(TLB_NEW_AREA, tlb_handler);
	init_area(TRAP_NEW_AREA, trap_handler);
	init_area(SYSCALL_NEW_AREA, syscall_handler);
}

void init_pcbs(pcb_t tests[]){
	int i;
	for (i=0; i<3; i++){
		setBit(IEc,&(tests[i].p_s.status),1);
		setBit(VMc,&(tests[i].p_s.status),0);
		setBit(TE,&(tests[i].p_s.status),1);
		setBit(KUc,&(tests[i].p_s.status),0);
		tests[i].p_s.reg_sp = RAMTOP-FRAMESIZE*(i+1);
		tests[i].priority = i+1;
	}
	tests[0].p_s.pc_epc = (unsigned int) test1;
	tests[1].p_s.pc_epc = (unsigned int) test2;
	tests[2].p_s.pc_epc = (unsigned int) test3;
}

void init(pcb_t *ready_queue, pcb_t tests[]){
	initPcbs();
	LIST_HEAD(&(ready_queue->p_next));
	init_areas();
	init_pcbs(tests);
	int i;
	for (i=0; i<3; i++) list_add(&(tests[i].p_next),&(ready_queue->p_next));
}
