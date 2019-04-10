#include "scheduler.h"
#include "listx.h"
#include "types_rikaya.h"
#include <umps/libumps.h>

void scheduler(struct list_head* head){
	struct list_head* scroll = head->prev;
	state_t* tmp_state=NULL;
	if (getTIMER()>2000 && getTIMER()<3000 && current!=NULL){
		/*il processo nel processore va reimpostato alla sua pr originale, lo rimetti in coda, rifai il calcolo del max poi aging degli altri*/
	current->priority = current->original_priority;
	insertProcQ(head,current);
	current=removeProcQ(head);
	log_process_order((int)(current->original_priority));
	increment_pcbs_priority(head);
	setTIMER(3000);
	log_process_order((int)(current->original_priority));
	LDST(tmp_state);
		}
	else{
		scheduler_init(head);
		}
}

void scheduler_init(struct list_head* head){
	current=removeProcQ(head);
	log_process_order((int)(current->original_priority));
	increment_pcbs_priority(head);
	setTIMER(3000);
	setSTATUS(getSTATUS()|1);
	LDST(&(current->p_s));
}

void increment_pcbs_priority(struct list_head* head){
		struct list_head* scroll = head->next;
		list_for_each(scroll,head){
			container_of(scroll,pcb_t,p_next)->priority = container_of(scroll,pcb_t,p_next)->priority+1;
		}
}
