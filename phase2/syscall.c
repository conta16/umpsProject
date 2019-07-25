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
	current->last_syscall_time = getClock();
	current->total_time_user = (current->last_syscall_time - current->middle_time);
	state_t* old=(state_t*)SYSCALL_OLD_AREA; //old punta all'old-area

	switch (old->reg_a0){
	case GETCPUTIME:
		getTime(old->reg_a1, old->reg_a2, old->reg_a3);
		break;
	case CREATEPROCESS:
		createProcess(old->reg_a1, old->reg_a2, old->reg_a3);
		break;
	case TERMINATEPROCESS: // se il tipo di chiamata è 3, chiamiamo il gestore deputato, poi lo scheduler
		terminateProcess(old->reg_a1);
		scheduler(&(ready_queue.p_next));
		break;
	case IOCOMMAND:
		ioCommand(old->reg_a1, old->reg_a2, old->reg_a3);
		break;
	case SETTUTOR:
		setTutor();
		break;
	case GETPID:
		getPids(old->reg_a1, old->reg_a2);
	  break;

	default: //in ogni altro caso, errore.
		syscall_error();
		break;
		}
		current->total_time_kernel += (getClock() - current->last_syscall_time);
		current->middle_time = getClock();
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

void getPids(void ** pid, void ** ppid){
	if (*pid != NULL)
		*pid = current;
	if (*ppid != NULL)
		*ppid = current->p_parent;

}

void getTime (unsigned int *user, unsigned int *kernel, unsigned int *wallclock){
	if (user != NULL && kernel != NULL && wallclock != NULL){
		*user = current->total_time_user;
		*kernel = current->total_time_kernel + (getClock()-current->last_syscall_time);
		*wallclock = (getClock() - current->initial_time);
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
		if(b == 0){
			if (dev < 32 || (dev < 40 && type == 0)) SYSCALL(PASSEREN,&(keys[dev]),0,0);
			else SYSCALL(PASSEREN,&(keys[dev+8]),0,0);
		}
	}while(b == 1);
}
int createProcess( state_t *statep, int priority, void ** cpid){
	pcb_t* p = allocPcb();
	if (p == NULL) return -1;
		insertChild(current,p);
	p->p_s = statep;
	p->priority = priority;
	p->original_priority = priority;
	cpid = &p;
	return 0;
	}
void setTutor(){
		current->tutor = 1;
	}
	int get_process(void **pid, struct list_head children){
        struct list_head *tmp;
        list_for_each(tmp,&children){
                if (container_of(tmp,pcb_t,p_sib) == pid) return 0;
                if (!emptyChild(tmp)) get_process(pid,tmp->p_child);
        }
        return -1;
}

void kill_proc(void **pid){
        struct list_head *tmp;
        struct list_head children = pid->p_child;
        pcb_t* tutor = find_tutor(pid);
        freePcb(pid);
        list_for_each(tmp,&children){
                insertChild(tutor,tmp);
        }
}

pcb_t* find_tutor(void **pid){
        if (pid->p_parent == NULL) return pid; /se ammazzi la radice dell'albero genealogico, sono problemi/
        if (pid->tutor == 1) return pid;
        else find_tutor(pid->p_parent);
}
void terminateProcess(void **pid){
        if (pid != 0 || pid != NULL){
                if (get_process(pid,current->p_child) == -1) return -1;
                kill_proc(pid);
        }
        else kill_proc(current);
        return 0;
}
}
