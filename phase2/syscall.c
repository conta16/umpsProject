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

	switch (old->reg_a0){
	case GETCPUTIME:
		getTime(old->reg_a1, old->reg_a2, old->reg_a3);
		break;
	case TERMINATEPROCESS: // se il tipo di chiamata è 3, chiamiamo il gestore deputato, poi lo scheduler
		terminateProcess(old->reg_a1);
		scheduler(&(ready_queue.p_next));
		break;
	case IOCOMMAND:
		ioCommand(old->reg_a1, old->reg_a2, old->reg_a3);
		break;
	case GETPID:
		getPids(old->reg_a1, old->reg_a2);
	  break;
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

void terminateProcess(void **pid){ //Gestore della systeamcall 3.
	pcb_t *tmp;
	/*come da SYSCALL3 della phase2 se il pid (suppongo il puntatore e non il suo valore)
		è zero allora termino il processo corrente*/
	if (*pid == 0){
		tmp = current;
		current = NULL;
	}
	else
		tmp = *pid;
	recursive_termination(tmp); //Termina il processo corrente e la progenie
	}

void recursive_termination(pcb_t* pcb) {
	while (!emptyChild(pcb)) //chiamata ricorsiva su tutta la progenie
		recursive_termination(removeChild(pcb)); //Ogni nodo chiama la funzione sul primo figlio, rimuovendolo già dalla lista dei figli
	outProcQ(&(ready_queue.p_next), pcb); //nel caso il processo si trovi in ready_queue, viene rimosso (sostanzialmente inutile, ma previsto dalla specifica)
	freePcb(pcb); // Il pcb viene aggiunto alla lista dei processi liberi.
}
void getPids(void ** pid, void ** ppid){
	if (*pid != NULL)
		*pid = current;
	if (*ppid != NULL)
		*ppid = current->p_parent;

}

void getTime (unsigned int *user, unsigned int *kernel, unsigned int *wallclock){
	if (user != NULL && kernel != NULL && wallclock != NULL){
		*user = (current->utp_time/100);
		*kernel = (current->ktp_time/100);
		*wallclock = ((*kernel + *user)/100);
	}
	else
	{
		syscall_error();
	}
}
int ioCommand(unsigned int command, unsigned int *register, int type){
	int dev = check_terminal(register);
	if (dev < 32)
		register->command = command;
	else
		if (type == 0)
			register->transm_command = command;
		else
			register->recv_command = command;
	do{
		int b = insertBlocked(&(keys[dev]),current);
		if (dev < 32 || (dev < 40 && type == 0)) SYSCALL(PASSEREN,&(keys[dev]),0,0);
		else SYSCALL(PASSEREN,&(keys[dev+8]),0,0);
	}while(b == 1);

}
