#include <umps/libumps.h>
#include "pcb.h"
#include "asl.h"
#include "syscall.h"
#include "scheduler.h"
#include "utils.h"
#include "const_rikaya.h"
#include "arch.h"
#include "asl.h"

extern pcb_t* current;
extern pcb_t ready_queue;

extern unsigned int keys[49];

short int spec_assigned[3];

state_t *sysbk_old, *tlb_old, *pgmtrap_old;
state_t *sysbk_new, *tlb_new, *pgmtrap_new;

void syscall_handler(){
	current->last_syscall_time = getClock();
	current->total_time_user = (current->last_syscall_time - current->middle_time);
	state_t* old=(state_t*)SYSBK_OLDAREA; //old punta all'old-area
	switch (old->reg_a0){
	case GETCPUTIME:
		get_time(old->reg_a1, old->reg_a2, old->reg_a3);
		break;
	case CREATEPROCESS:
		create_process(old->reg_a1, old->reg_a2, old->reg_a3);
		break;
	case TERMINATEPROCESS: // se il tipo di chiamata è 3, chiamiamo il gestore deputato, poi lo scheduler
		terminate_process((void **)(old->reg_a1));
		scheduler(&(ready_queue.p_next));
		break;
	case WAITIO:
		io_command(old->reg_a1, old->reg_a2, old->reg_a3);
		break;
	case SETTUTOR:
		set_tutor();
		break;
	case GETPID:
		get_pids(old->reg_a1, old->reg_a2);
	  break;
	case VERHOGEN:
		verhogen(old->reg_a1);
		sys_return(old);
		break;
	case PASSEREN:
		passeren(old->reg_a1);
		sys_return(old);
		break;
	case WAITCLOCK:
		wait_clock();
	case SPECPASSUP:
		spec_passup(old->reg_a1, old->reg_a1, old->reg_a2, old->reg_a3);
	default: //in ogni altro caso, errore.
		syscall_error();
		break;
		}
		current->total_time_kernel += (getClock() - current->last_syscall_time);
		current->middle_time = getClock();
	}

void sys_return(state_t* old){ //Questa syscall aggiorna il valore di pc e fa ldst dell'area old in modo da far ripartire l'esecuzione
	old->pc_epc+=4;
	old->reg_t9=old->pc_epc;
	LDST(old);
}

int check_device(unsigned int *reg){
	int i,dev;
	for (dev=3; dev<8; dev++)
		for (i=0; i<8; i++)
			if (DEV_REG_ADDR(dev,i) == (unsigned int)reg)
				return i+((dev-3)*8);
	return -1;
}

void syscall_error(){
	PANIC();
}

void oldarea_pc_increment(){ //utility: affinchè dopo la syscall, il processo chiamante possap prosegire, occorre incrementare il pc di tale processo. Non ancora utilizzata.
	state_t* old=(state_t*)SYSBK_OLDAREA;
	old->reg_t9+=4; //per regioni architetturali, un incremento di pc non è valideo se non viene anche incrementato t9.
	old->pc_epc+=4;
}

void wait_clock(){
	unsigned int *tmp = (unsigned int *)I_TIMER;
	*tmp = (unsigned int) 1;
	SYSCALL(PASSEREN,(unsigned int)&(keys[48]),0,0);
}

void get_pids(void ** pid, void ** ppid){
	if (*pid != NULL)
		*pid = current;
	if (*ppid != NULL)
		*ppid = current->p_parent;
}

void get_time (unsigned int *user, unsigned int *kernel, unsigned int *wallclock){
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
unsigned int io_command(unsigned int command, unsigned int *ourReg, int type){
	dtpreg_t* devreg = (dtpreg_t*) ourReg;
	termreg_t* termreg = (termreg_t*) ourReg;
	unsigned int semd_id;
	int dev = check_device(ourReg);
	if (dev < 32)
		devreg->command = command;
	else
		if (type == 0)
			termreg->transm_command = command;
		else
			termreg->recv_command = command;
	do{
		if (dev < 32 || (dev < 40 && type == 0))
			semd_id = (unsigned int)&(keys[dev]);
		else
			semd_id = (unsigned int)&(keys[dev+8]);
		SYSCALL(PASSEREN,semd_id,0,0);
	}while(getSemd((int *)semd_id) == NULL);
	if (dev < 32) return devreg->status;
	else if (type == 0) return termreg->transm_status;
	else return termreg->recv_status;
}
int create_process( state_t *statep, int priority, void ** cpid){
	int i;
	pcb_t* p = allocPcb();
	if (p == NULL) return -1;
	insertChild(current,p);
	char *c = (char *)&(p->p_s);
	char *d = (char *)&(statep);
        for (i=0; i<sizeof(state_t); i++,c++,d++)	/* Ciclo di scorrimento per copiare il campo statep in p_s*/
                *c=*d;
	p->priority = priority;
	p->original_priority = priority;
	cpid = (void **)p; 				/*---------- dubbio --------------*/
	return 0;
}
void set_tutor(){
		current->tutor = 1;
}
int get_process(void **pid, struct list_head children){
        struct list_head *tmp;
        list_for_each(tmp,&children){
                if (container_of(tmp,pcb_t,p_sib) == (pcb_t *)pid) return 0;
                if (!emptyChild(container_of(tmp,pcb_t,p_sib))) get_process(pid,container_of(tmp,pcb_t,p_sib)->p_child);
        }
        return -1;
}

void kill_proc(void **pid){
	pcb_t* proc = (pcb_t*) *pid;
        struct list_head *tmp;
        struct list_head children = proc->p_child;
        pcb_t* tutor = find_tutor(proc);
        freePcb(proc);
        list_for_each(tmp,&children){
                insertChild(tutor,container_of(tmp,pcb_t,p_sib));
        }
}

pcb_t* find_tutor(pcb_t* pid){
        if (pid->p_parent == NULL) return pid;
        if (pid->tutor == 1) return pid;
        else return find_tutor(pid->p_parent);
}
int terminate_process(void **pid){
        if (pid != 0 || pid != NULL){
                if (get_process(pid,current->p_child) == -1) return -1;
                kill_proc(pid);
        }
        else kill_proc((void **)current);
        return 0;
}

void verhogen(int* semaddr) {
	if (*semaddr>=0)
		*semaddr+=1;
	else{
		pcb_t* tmp=removeBlocked(semaddr);
		if (tmp!=NULL)
			insertProcQ(&(ready_queue.p_next), tmp);
	}
}
void passeren(int* semaddr){
		*semaddr-=1;
		if (*semaddr<0){
			insertBlocked(semaddr, current);
			outProcQ(&(ready_queue.p_next), current);
			current = NULL;
			/*insertProcQ(&(blocked_queue.p_next), current);*/
			scheduler(&(ready_queue.p_next));
		}
}

int spec_passup(int type, state_t* old, state_t* new){
	if(spec_assigned[type]){
		return -1;
	}
	spec_assigned[type]=1;
	switch (type) {
		case 0:
			sysbk_old=old;
			sysbk_new=new;
			break;
		case 1:
			tlb_old=old;
			tlb_new=new;
			break;
		case 2:
			pgmtrap_old=old;
			pgmtrap_new=new;
			break;
	}
	return 0;
}
