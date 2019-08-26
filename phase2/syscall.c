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

static state_t* old=(state_t*)SYSBK_OLDAREA;

static int syscall;

void syscall_handler(){
	if (current != NULL){
		current->last_syscall_time = getClock();
		current->total_time_user = (current->last_syscall_time - current->middle_time);
	}
	syscall=old->reg_a0;
	switch (syscall){
	case GETCPUTIME:
		oldarea_pc_increment();
		get_time(old->reg_a1, old->reg_a2, old->reg_a3);
		break;
	case CREATEPROCESS:
		create_process(old->reg_a1, old->reg_a2, old->reg_a3);
		sys_return(old);
		break;
	case TERMINATEPROCESS: // se il tipo di chiamata è 3, chiamiamo il gestore deputato, poi lo scheduler
		terminate_process((void **)(old->reg_a1));
		scheduler(&(ready_queue.p_next));
		break;
	case WAITIO:
		io_command(old->reg_a1, old->reg_a2, old->reg_a3);
		sys_return(old);
		break;
	case SETTUTOR:
		set_tutor();
		break;
	case GETPID:
		oldarea_pc_increment();
		get_pids(old->reg_a1, old->reg_a2);
	  break;
	case VERHOGEN:
		oldarea_pc_increment();
		verhogen(old->reg_a1);
		break;
	case PASSEREN:
	  oldarea_pc_increment();
		passeren(old->reg_a1, old);
		break;
	case WAITCLOCK:
		oldarea_pc_increment();
		wait_clock();
		break;
	case SPECPASSUP:
		oldarea_pc_increment();
		spec_passup(old->reg_a1, old->reg_a2, old->reg_a3);
		break;
	default: //in ogni altro caso, errore.
		syscall_error();
		break;
		}
	if (current != NULL){
		current->total_time_kernel += (getClock() - current->last_syscall_time);
		current->middle_time = getClock();
	}
	LDST(old);
}

void sys_return(state_t* old){ //aggiorna il valore di pc e fa ldst dell'area old in modo da far ripartire l'esecuzione
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
	old->pc_epc+=4;
	old->reg_t9=old->pc_epc; //per regioni architetturali, un incremento di pc non è valideo se non viene anche incrementato t9.
}

void wait_clock(){
	unsigned int *tmp = (unsigned int *)I_TIMER;
	*tmp = (unsigned int) 100000*TIME_SCALE;
	passeren((unsigned int)&(keys[48]), (state_t*) SYSBK_OLDAREA);
}

void get_pids(void ** pid, void ** ppid){
	if (pid != NULL)
		*pid = current;
	if (ppid != NULL)
		*ppid = current->p_parent;
}

void get_time (unsigned int *user, unsigned int *kernel, unsigned int *wallclock){
	if (user != NULL)
		*user = current->total_time_user;
	if (kernel != NULL)
		*kernel = current->total_time_kernel + (getClock()-current->last_syscall_time);
	if (wallclock != NULL)
		*wallclock = (getClock() - current->initial_time);
}

void io_command(unsigned int command, unsigned int *ourReg, int type){
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
	if (dev < 32 || (dev < 40 && type == 0))
		semd_id = (unsigned int)&(keys[dev]);
	else
		semd_id = (unsigned int)&(keys[dev+8]);
	if (dev < 32) old->reg_v0 = devreg->status;
	else if (type == 0) old->reg_v0 = termreg->transm_status;
	else old->reg_v0 = termreg->recv_status;
	oldarea_pc_increment();
	passeren(semd_id, (state_t*) SYSBK_OLDAREA);
}

void create_process(state_t *statep, int priority, void ** cpid){
	int i;
	pcb_t* p = allocPcb();
	if (p == NULL){
		old->reg_v0 = -1;
		return;
	}
	insertChild(current,p);
	insertProcQ(&(ready_queue.p_next), p);
	char *c = (char *)&(p->p_s);
	char *d = (char *)statep;
  for (i=0; i<sizeof(state_t); i++,c++,d++)	/* Ciclo di scorrimento per copiare il campo statep in p_s*/
    *c=*d;
	p->priority = priority;
	p->original_priority = priority;
	if (cpid!=NULL) *cpid = (void **)p; 				/*---------- dubbio --------------*/
	old->reg_v0 = 0;
}
void set_tutor(){
		current->tutor = 1;
}
int get_process(void **pid, struct list_head children){
        struct list_head *tmp;
        list_for_each(tmp,&children){
                if (container_of(tmp,pcb_t,p_sib) == (pcb_t *) pid) return 0;
                if (!emptyChild(container_of(tmp,pcb_t,p_sib))) get_process(pid,container_of(tmp,pcb_t,p_sib)->p_child);
        }
        return -1;
}

void kill_proc(pcb_t* pid/*void **pid*/){
	pcb_t* proc = /*(pcb_t*) */ pid;
	pcb_t* tmp;
  pcb_t* tutor = find_tutor(proc);
	while(!list_empty(&(proc->p_child))){
		tmp = removeChild(proc);
		tmp->p_parent = NULL;
    insertChild(tutor,tmp);
	}
	outProcQ(&(ready_queue.p_next),proc);
	if (proc == current) current = NULL;
        freePcb(proc);
}

pcb_t* find_tutor(pcb_t* pid){
        if (pid->p_parent == NULL) return pid;
        if (pid->tutor == 1) return pid;
        else return find_tutor(pid->p_parent);
}
int terminate_process(void **pid){
        if (pid != 0 && pid != NULL){
                if (get_process(pid,current->p_child) == -1) return -1;
                kill_proc((pcb_t *) *pid);
        }
        else kill_proc(/*(void **)*/current);
        return 0;
}

void verhogen(int* semaddr) {
  pcb_t* tmp;
	*semaddr+=1;
	tmp=removeBlocked(semaddr);
	if (tmp!=NULL)
		insertProcQ(&(ready_queue.p_next), tmp);
}

void passeren(int* semaddr, state_t* block_state){
		*semaddr-=1;
		if (*semaddr<0){
			copyState(block_state, &(current->p_s));
			insertBlocked(semaddr, current);
			outProcQ(&(ready_queue.p_next), current);
			current->total_time_kernel += (getClock() - current->last_syscall_time);
			current->middle_time = getClock();
			current = NULL;
			/*insertProcQ(&(blocked_queue.p_next), current);*/
			scheduler(&(ready_queue.p_next));
		}
}

int spec_passup(int type, state_t* old, state_t* new){
	if(current->spec_assigned[type]){
		return -1;
	}
	current->spec_assigned[type]=1;
	switch (type) {
		case 0:
			current->sysbk_old=old;
			current->sysbk_new=new;
			break;
		case 1:
			current->tlb_old=old;
			current->tlb_new=new;
			break;
		case 2:
			current->pgmtrap_old=old;
			current->pgmtrap_new=new;
			break;
	}
	return 0;
}
