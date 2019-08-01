
/*
    Copyright (C) 2019 Davide Davoli, Simone Branchetti, Michele Luca Contalbo

    This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License$

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General$

    You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>
*/


#include "listx.h"
#include "const.h"
#include "pcb.h"
#include "asl.h"
#include "utils.h"
#include "init.h"
#include "const_rikaya.h"

extern void test();
extern void wait();
extern pcb_t* test_pcb;
extern pcb_t* idle;
extern void syscall_handler();
extern void int_handler();
extern void tlb_handler();
extern void sysbk_handler();
extern void pgmtrap_handler();

void init_area(state_t* newarea, void (*handler)()){

	/*Casting dell'indirizzo della new area in state_t*, usato successivamente per settare determinati bit*/

	state_t *tmp = newarea;
	tmp->pc_epc = (int)handler;
	tmp->reg_t9 = tmp->pc_epc;
	tmp->reg_sp = RAMTOP;
	setBit(KUc,&(tmp->status),0);
	setBit(IEc,&(tmp->status),0);
	setBit(VMc,&(tmp->status),0);
	setBit(TE,&(tmp->status),1);
	tmp->status|=(255<<8); /*Tutti i bit di interrupt sono messi a 0: una volta caricato lo stato della new area, quindi, nessun nuovo interrupt verrà considerato
				    finchè non verrà caricato un nuovo pcb (che ha i bit di interrupt tutti abilitati tranne l'ultimo)*/
}

void init_areas(){
	init_area((state_t*)INT_NEWAREA, int_handler);
	init_area((state_t*)SYSBK_NEWAREA, sysbk_handler);
	init_area((state_t*)TLB_NEWAREA, tlb_handler);
	init_area((state_t*)PGMTRAP_NEWAREA, pgmtrap_handler);
}

void init_pcbs(){
	/*allocPcb prende un pcb da pcbFree e setta in maniera opportuna tutti i campi tranne quello di stato*/
	test_pcb = allocPcb();
	/*Uso setBit (definito in utils.c) per settare i bit di stato IEc, VMc, TE, KUc*/
	setBit(IEc,&(test_pcb->p_s.status),1);
	setBit(VMc,&(test_pcb->p_s.status),0);
	setBit(TE,&(test_pcb->p_s.status),1);
	setBit(KUc,&(test_pcb->p_s.status),0);
	test_pcb->p_s.status|=(255<<8);
					/*Abilito tutti gli interrupt tranne quelli da terminale (che verrà messo a posto nella prossima fase).
					E' disabilitato perchè non mandando l'ack dell'interrupt, appena viene caricato il nuovo pcb da scheduler, il controllo viene
					ancora passato all'interrupt handler e così via*/
	test_pcb->p_s.status|=(1UL<<0);
	test_pcb->p_s.status|=(0x000082001UL<<2); //LDST() fa un push all'indietro dei bit IE, dunque per settare l'IEc occorre settare anche IEp.
	test_pcb->p_s.reg_sp = RAMTOP-FRAMESIZE;
	test_pcb->priority = 1;
	test_pcb->original_priority= 1; /*aggiunto il campo original_priority per implementare aging*/

	/*Per ogni pcb, faccio puntare il campo pc a una delle tre funzioni test1, test2 e test3*/

	test_pcb->p_s.pc_epc = (unsigned int) test;
	test_pcb->p_s.reg_t9 = test_pcb->p_s.pc_epc;

	idle = allocPcb();

        setBit(IEc,&(idle->p_s.status),1);
        setBit(VMc,&(idle->p_s.status),0);
        setBit(TE,&(idle->p_s.status),1);
        setBit(KUc,&(idle->p_s.status),0);
        idle->p_s.status|=(255<<8);

        idle->p_s.status|=(1UL<<0);
        idle->p_s.status|=(1UL<<2); //LDST() fa un push all'indietro dei bit IE, dunque per settare l'IEc occorre settare anche IEp.
        idle->p_s.reg_sp = RAMTOP-FRAMESIZE;
        idle->priority = 1;
        idle->original_priority= 1; /*aggiunto il campo original_priority per implementare aging*/

        /*Per ogni pcb, faccio puntare il campo pc a una delle tre funzioni test1, test2 e test3*/

        idle->p_s.pc_epc = (unsigned int) wait;
        idle->p_s.reg_t9 = idle->p_s.pc_epc;


}

/*Funzione chiamata dal main. Inizializza i pcb e le new area chiamando funzioni che settano i bit di stato e gli altri campi necessari per l'inizializzazione del sistema
Infine aggiunge, con un ciclo, i pcb alla ready_queue */

void init(pcb_t *ready_queue){
	initPcbs();
	initASL();
	mkEmptyProcQ(&(ready_queue->p_next));
	init_areas();
	init_pcbs();
	insertProcQ(&(ready_queue->p_next), test_pcb);
}
