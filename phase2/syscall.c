#include <umps/libumps.h>
#include "syscall.h"
#include "scheduler.h"
#include "utils.h"
#include "pcb.h"

extern pcb_t* current;
extern pcb_t ready_queue;
extern void adderrbuf();
unsigned int keys[48]; /* da sistemare in posto opportuno e inizializzarlo per bene*/

void syscall_handler(){
	int old_time = /*3000-*/getTIMER();
	int new_time;
	state_t* old=(state_t*)SYSCALL_OLD_AREA; //old punta all'old-area

	if (getExcCode()!=8) //se il codice dell'eccezione è diverso da quello da gestire, kernel panic.
		PANIC();
	switch (old->reg_a0){
	case SYS1: /*chiamo la SYS1 per ritornare il tempo passato in questo processo*/
		countTime();
		break;
	case SYS3: // se il tipo di chiamata è 3, chiamiamo il gestore deputato, poi lo scheduler
		terminateProcess();
		new_time = /*3000-*/getTIMER();
		current->ktp_time += new_time - old_time;
		current->lkp_time = new_time - old_time;
		scheduler(&(ready_queue.p_next));
		break;
	case IOCOMMAND:
		int dev = check_terminal(old->reg_a2);
		if (dev < 32)
			old->reg_a2->command = old->reg_a1;
		else
			if (old->reg_a3 == 0)
				old->reg_a2->transm_command = old->reg_a1;
			else
				old->reg_a2->recv_command = old->reg_a1;
		do{
			int b = insertBlocked(&(keys[dev]),current);
			if (dev < 32 || (dev < 40 && old->reg_a3 == 0)) SYSCALL(PASSEREN,&(keys[dev]),0,0);
			else SYSCALL(PASSEREN,&(keys[dev+8]),0,0);
		}while(b == 1);
	default: //in ogni altro caso, errore.
		syscall_error();
		break;
		}

	}

int check_device(unsigned int *reg){
	int i,dev;
	for (dev=3; dev<8; dev++)
		for (i=0; i<8; i++)
			if (DEV_REG_ADDR(dev,i) == reg)
				return i+((dev-3)*8);
	return -1;
}

void syscall_error(){
	adderrbuf("syscallerror");
}

void oldarea_pc_increment(){ //utility: affinchè dopo la syscall, il processo chiamante possap prosegire, occorre incrementare il pc di tale processo. Non ancora utilizzata.
	state_t* old=(state_t*)SYSCALL_OLD_AREA;
	old->reg_t9+=4; //per regioni architetturali, un incremento di pc non è valideo se non viene anche incrementato t9.
	old->pc_epc+=4;
}

void terminateProcess(){ //Gestore della systeamcall 3.
	pcb_t *tmp= current;
	current=NULL;
	recursive_termination(tmp); //Termina il processo corrente e la progenie
	}


void recursive_termination(pcb_t* pcb) {
	while (!emptyChild(pcb)) //chiamata ricorsiva su tutta la progenie
		recursive_termination(removeChild(pcb)); //Ogni nodo chiama la funzione sul primo figlio, rimuovendolo già dalla lista dei figli
	outProcQ(&(ready_queue.p_next), pcb); //nel caso il processo si trovi in ready_queue, viene rimosso (sostanzialmente inutile, ma previsto dalla specifica)
	freePcb(pcb); // Il pcb viene aggiunto alla lista dei processi liberi.
}
