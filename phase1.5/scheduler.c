#include "scheduler.h"
#include "listx.h"
#include "types_rikaya.h"
#include <umps/libumps.h>

void scheduler(struct list_head* head){
	struct list_head* scroll = head->prev;
	state_t* tmp_state=NULL;
	if (getTIMER()>2 && getTIMER()<3){
			/*il processo nel processore va reimpostato alla sua pr originale, lo rimetti in coda, rifai il calcolo del max poi aging degli altri*/
			current->priority = current->original_priority;
			insertProcQ(head,current);
			tmp_state=&(container_of(scroll,pcb_t,p_next)->p_s);
			current = container_of(scroll,pcb_t,p_next);
			list_del(scroll);
			scroll = head->next;
			list_for_each(scroll,head){
				container_of(scroll,pcb_t,p_next)->priority = container_of(scroll,pcb_t,p_next)->priority+1;
			}
	setTIMER(3);
	log_process_order((int)(current->original_priority));
	LDST(tmp_state);
	}
else
	{scheduler_init(head);}


}

void scheduler_init(struct list_head* head){
		struct list_head* scroll = head->prev;
		state_t* tmp_state=&(container_of(scroll,pcb_t,p_next)->p_s);
		current = container_of(scroll,pcb_t,p_next);
		list_del(scroll);
		scroll = head->next;
		list_for_each(scroll,head){
			container_of(scroll,pcb_t,p_next)->priority = container_of(scroll,pcb_t,p_next)->priority+1;
		}
		
	setTIMER(3);
	log_process_order((int)(current->original_priority));
	LDST(tmp_state);
}
