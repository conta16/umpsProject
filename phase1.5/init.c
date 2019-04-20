#include "listx.h"
#include "const.h"
#include "pcb.h"
#include "asl.h"
#include "utils.h"
#include "init.h"


pcb_t *tests[TEST_PROCS];
pcb_t ready_queue;


extern void test1();
extern void test2();
extern void test3();

extern void syscall_handler();
extern void int_handler();

void init_area(state_t* newarea, void (*handler)()){
	state_t *tmp = newarea;
	tmp->pc_epc = (int)handler;
	tmp->reg_t9 = tmp->pc_epc;
	tmp->reg_sp = RAMTOP;
	setBit(KUc,&(tmp->status),0);
	setBit(IEc,&(tmp->status),0);
	setBit(VMc,&(tmp->status),0);
	setBit(TE,&(tmp->status),1);
	tmp->status&=(~(255)<<8);
}

void init_areas(){
	init_area((state_t*)INT_NEW_AREA, int_handler);
	init_area((state_t*)TLB_NEW_AREA, tlb_handler);
	init_area((state_t*)TRAP_NEW_AREA, trap_handler);
	init_area((state_t*)SYSCALL_NEW_AREA, syscall_handler);
}

void init_pcbs(pcb_t *tests[]){
	int i;
	for (i=0; i<3; i++){
		tests[i] = allocPcb();
		setBit(IEc,&(tests[i]->p_s.status),1); //settaggio di IEc a 1: ineterrupt abilitati
		setBit(VMc,&(tests[i]->p_s.status),0); //settaggio di VM a 0: viltual memory disabilitata
		setBit(TE,&(tests[i]->p_s.status),1);
		setBit(KUc,&(tests[i]->p_s.status),0); //settaggio di user mode
		tests[i]->p_s.status|=(127<<8); // Settaggio dell'interrupt mask del processo. Tutti gli interrupt sono attivati tranne quello del terminale
		tests[i]->p_s.status|=(1UL<<2); //LDST() fa un push all'indietro dei bit IE, dunque per settare l'IEc occorre settare anche IEp.
		tests[i]->p_s.reg_sp = RAMTOP-FRAMESIZE*(i+1);//Settaggio come da specifica dello stack pointer del processo
		tests[i]->priority = i+1; //Settaggio come da specificadei due campi relativi le priorità
		tests[i]->original_priority= i+1;
	}
	tests[0]->p_s.pc_epc = (unsigned int) test1; //settaggio di t9 e pc all'indirizzo di test-i
	tests[0]->p_s.reg_t9 = tests[0]->p_s.pc_epc;
	tests[1]->p_s.pc_epc = (unsigned int) test2;
	tests[1]->p_s.reg_t9 = tests[1]->p_s.pc_epc;
	tests[2]->p_s.pc_epc = (unsigned int) test3;
	tests[2]->p_s.reg_t9 = tests[2]->p_s.pc_epc;
}

void init(pcb_t *ready_queue, pcb_t *tests[]){
	initPcbs();
	mkEmptyProcQ(&(ready_queue->p_next));
	init_areas();
	init_pcbs(tests);
	int i;
	for (i=0; i<3; i++) insertProcQ(&(ready_queue->p_next), tests[i]);
}
