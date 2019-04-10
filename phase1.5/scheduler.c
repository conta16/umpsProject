#include "scheduler.h"
#include "listx.h"
#include "types_rikaya.h"
#include <umps/libumps.h>

void scheduler(struct list_head* head){
	struct list_head* scroll = head->next;
	struct list_head* del;
	state_t* tmp_state=NULL;
	int maxpr = -1;
	int i;
	if (getTIMER()>2 && getTIMER()<3){
			/*il processo nel processore va reimpostato alla sua pr originale, lo rimetti in coda, rifai il calcolo del max poi aging degli altri*/
			for (i=0;i<3;i++){
				if (current == (OrVec[i].proc))
				current->priority = OrVec[i].original;
			}
			insertProcQ(head,current);
			while (!list_is_last(head, scroll))
				{
					if (container_of(scroll,pcb_t,p_next)->priority > maxpr) {maxpr = container_of(scroll,pcb_t,p_next)->priority;}
					scroll = scroll->next;
				}
			scroll = head->next;
			while (!list_is_last(head,scroll)){ /*scazza se il primo elemento è quello da inserire nel processore*/
				if(container_of(scroll,pcb_t,p_next)->priority == maxpr){
				tmp_state=&(container_of(scroll,pcb_t,p_next)->p_s);
				LDST(tmp_state);
				del = scroll;
				/*scroll = scroll->prev;*/
				current = container_of(del,pcb_t,p_next); /*aggiorna la var globale per davolz*/
				list_del(del);					
				}
			scroll = scroll->next;
			}
			scroll = head->next;
			while (!list_is_last(head,scroll)){container_of(scroll,pcb_t,p_next)->priority = container_of(scroll,pcb_t,p_next)->priority+1;}	
	setTIMER(3);
	}
else
	{scheduler_init(head);}


}

void scheduler_init(struct list_head* head){
		struct list_head* scroll = head->next;
		struct list_head* del;
		int maxpr = -1;
		state_t* tmp_state=NULL;
		while (!list_is_last(head,scroll)){
			if (container_of(scroll,pcb_t,p_next)->priority > maxpr){maxpr = container_of(scroll,pcb_t,p_next)->priority;}	
			scroll = scroll->next;
		}
		scroll = head->next;
		while (!list_is_last(scroll, head)){
			if (container_of(scroll,pcb_t,p_next)->priority == maxpr){
			tmp_state=&(container_of(scroll,pcb_t,p_next)->p_s);
			LDST(tmp_state);
			del = scroll;
			current = container_of(del,pcb_t,p_next);
				scroll = scroll->prev;
				list_del(del);}
		}
	setTIMER(3);

}
