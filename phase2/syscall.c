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
		get_time((unsigned int *)old->reg_a1, (unsigned int *)old->reg_a2, (unsigned int *)old->reg_a3);
		break;
	case CREATEPROCESS:
		create_process((state_t * )old->reg_a1, old->reg_a2, (void **)old->reg_a3);
		sys_return(old);
		break;
	case TERMINATEPROCESS:
		oldarea_pc_increment();
		terminate_process((void **)(old->reg_a1));
		break;
	case WAITIO:
		io_command(old->reg_a1, (unsigned int *)old->reg_a2, old->reg_a3);
		sys_return(old);
		break;
	case SETTUTOR:
		oldarea_pc_increment();
		set_tutor();
		break;
	case GETPID:
		oldarea_pc_increment();
		get_pids((void **)old->reg_a1, (void **)old->reg_a2);
	  break;
	case VERHOGEN:
		oldarea_pc_increment();
		verhogen((int *)old->reg_a1);
		break;
	case PASSEREN:
	  	oldarea_pc_increment();
		passeren((int *)old->reg_a1, old);
		break;
	case WAITCLOCK:
		oldarea_pc_increment();
		wait_clock();
		break;
	case SPECPASSUP:
		oldarea_pc_increment();
		spec_passup(old->reg_a1,(state_t *) old->reg_a2,(state_t *) old->reg_a3);
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
	for (dev=3; dev<8; dev++) //itera su diverse categorie di device (terminal, printers ecc.)
		for (i=0; i<8; i++) //itera su device della stessa categoria
			if (DEV_REG_ADDR(dev,i) == (unsigned int)reg)
				return i+((dev-3)*8);
	return -1;
}

void syscall_error(){
	PANIC();
}

void oldarea_pc_increment(){ //utility: affinche' dopo la syscall, il processo chiamante possap prosegire, occorre incrementare il pc di tale processo. Non ancora utilizzata.
	state_t* old=(state_t*)SYSBK_OLDAREA;
	old->pc_epc+=4;
	old->reg_t9=old->pc_epc; //per regioni architetturali, un incremento di pc non e' valideo se non viene anche incrementato t9.
}

void wait_clock(){
	unsigned int *tmp = (unsigned int *)I_TIMER;
	*tmp = (unsigned int) 100000*TIME_SCALE;
	passeren((int *)&(keys[48]), (state_t*) SYSBK_OLDAREA);
}

void get_pids(void ** pid, void ** ppid){ 		//semplice copia dei valori dal pcb
	if (pid != NULL)
		*pid = current;
	if (ppid != NULL)
		*ppid = current->p_parent;
}

void get_time (unsigned int *user, unsigned int *kernel, unsigned int *wallclock){ //semplice copia dei valori dal pcb
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
	if (dev < 32) //non è un terminale
		devreg->command = command;
	else
		if (type == 0) //device di trasmissione caratteri del terminale
			termreg->transm_command = command;
		else
			termreg->recv_command = command;
	if (dev < 32 || (dev < 40 && type == 0))
		semd_id = (unsigned int)&(keys[dev]); //prendo id del semaforo su cui verrà bloccato il processo
	else
		semd_id = (unsigned int)&(keys[dev+8]);
	if (dev < 32) old->reg_v0 = devreg->status;
	else if (type == 0) old->reg_v0 = termreg->transm_status;
	else old->reg_v0 = termreg->recv_status;
	oldarea_pc_increment();
	passeren((int *)semd_id, (state_t*) SYSBK_OLDAREA);
}

void create_process(state_t *statep, int priority, void ** cpid){
	int i;
	pcb_t* p = allocPcb();
	if (p == NULL){ //nessun processo disponibile nella lista dei processi liberi
		old->reg_v0 = -1; //ritorna -1
		return;
	}
	insertChild(current,p);//il pcb viene aggiunto nella coda dei figli del processo current
	insertProcQ(&(ready_queue.p_next), p);//il pcb viene aggiunto alla ready queue
	char *c = (char *)&(p->p_s);
	char *d = (char *)statep;
  	for (i=0; i<sizeof(state_t); i++,c++,d++)	/* Ciclo di scorrimento per copiare il campo statep in p_s*/
    		*c=*d;
	p->priority = priority;
	p->original_priority = priority;
<<<<<<< HEAD
	if (cpid!=NULL) *cpid = (void **)p;
=======
	if (cpid!=NULL) *cpid = (void *)p; //cpid punta al processo puntato da p, che quindi potrà essere usato anche al di fuori della syscall
>>>>>>> fd6cbc7b0976e56ad24cc996eec5434357b8da7c
	old->reg_v0 = 0;
}
void set_tutor(){
		current->tutor = 1;
}

<<<<<<< HEAD
=======
/*get_process: visito gli antenati del processo che va terminato. Se si visita current allora ritorna 0, altrimenti -1*/

>>>>>>> fd6cbc7b0976e56ad24cc996eec5434357b8da7c
int get_process(pcb_t *pid){
	if (pid->p_parent == NULL) return -1;
	if (pid->p_parent == current) return 0;
	else return get_process(pid->p_parent);
}

void kill_proc(pcb_t* pid){
	pcb_t* proc = pid;
	pcb_t* tmp;
	pcb_t* tutor = find_tutor(proc); //ritorna il tutor, cioè il processo a cui vanno affidati i processi figli del processo terminato. Se non c'è un tutor, il tutor diventa il processo radice
	while(!list_empty(&(proc->p_child))){
		tmp = removeChild(proc);
		tmp->p_parent = NULL;
    		if(tutor != proc) insertChild(tutor,tmp); //i processi figli del processo terminato diventano figli del tutor
	}
	if (proc->p_semkey != NULL){ //se viene terminato un processo che è bloccato ad un semaforo, aumento il semaforo e lo tolgo dalla lista dei bloccati
		*(proc->p_semkey)+=1;
		outBlocked(proc);
	}
	else outProcQ(&(ready_queue.p_next),proc);
	list_del(&(proc->p_sib)); //tolgo il processo da una eventuale lista di processi fratelli
	outChild(proc); //tolgo il processo dai figli del padre
	if (proc == current){
		current = NULL;
	}
        freePcb(proc);
}

pcb_t* find_tutor(pcb_t* pid){
        if (pid->p_parent == NULL) return pid;
        if (pid->tutor == 1) return pid;
        else return find_tutor(pid->p_parent);
}
int terminate_process(void **pid){
        if (pid != 0 && pid != NULL){ //se il processo da terminare non è il corrente
                if (get_process((pcb_t*) *pid) == -1) old->reg_v0= (unsigned int)-1; //se il processo da terminare non è discendente di current, ritorna -1
                else{
			kill_proc((pcb_t *) *pid);
			old->reg_v0 = 0;
		}
       	}
        else {
				kill_proc(current);
				old->reg_v0 = 0;
				scheduler(&(ready_queue.p_next));
	}
	return 0;
}

//verhogen: libero un processo dalla coda dei processi bloccati con id del semaforo semaddr

void verhogen(int* semaddr) {
	pcb_t* tmp;
	*semaddr+=1;  //il valore del semaforo viene aumentato
	tmp=removeBlocked(semaddr);  //se c'e' un processo in coda, viene rimosso
	if (tmp!=NULL)
		insertProcQ(&(ready_queue.p_next), tmp); //il processo in coda viene aggiunto alla ready queue
}

//passeren: blocca un processo nel semaforo con id semaddr e chiama scheduler

void passeren(int* semaddr, state_t* block_state){
		*semaddr-=1;  //il semaforo viene decrementato
		if (*semaddr<0){
			copyState(block_state, &(current->p_s)); //lo stato di "riavvio" viene salvato
			insertBlocked(semaddr, current);         //il processo corrente viene aggiunto alla coda dei processi bliccati al semaforo
			//outProcQ(&(ready_queue.p_next), current);//se il processo e' nella ready queue, viene rimosso
			current->total_time_kernel += (getClock() - current->last_syscall_time); //contabilita' dei tempi
			current->middle_time = getClock();
			current = NULL;
<<<<<<< HEAD
			scheduler(&(ready_queue.p_next));
=======
			scheduler(&(ready_queue.p_next));  			//viene chiamato lo scheduler per avviare un nuovo processo
>>>>>>> fd6cbc7b0976e56ad24cc996eec5434357b8da7c
		}
}

//spec_passup: old e new diventano gli stati che vengono caricati quando c'è un'eccezione di tipo type

int spec_passup(int type, state_t* old, state_t* new){
	if(current->spec_assigned[type]){ //se e' gia' stata effettuata una spec_passup, la chiamata ha esito negativo
		old->reg_v0=-1;
		return -1;
	}
	current->spec_assigned[type]=1;
	switch (type) {		//altrimenti in base al valore type, vengono settate le nuove old_area e new_area
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
	old->reg_v0=0; //ed in tal caso l'esito della chiamata e' positivo
	return 0;
}
