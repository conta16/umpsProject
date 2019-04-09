void scheduler(list_head* head){
	if (getTIMER()>2 && getTIMER()<3){
			struct list_head* scroll = head->next;
			struct list_head* del;
			int maxpr = -1;
			int i;
			/*il processo nel processore va reimpostato alla sua pr originale, lo rimetti in coda, rifai il calcolo del max poi aging degli altri*/
			for (i=0;i<3;i++){
				if (current->p_s = (OrVec[i].proc)->p_s)
				current->priority = OrVec[i].original;
			}
			insertProcQ(head,current);
			while (!list_is_last(head, scroll))
				{
					if (container_of(scroll,pcb_t,s_next)->priority > maxpr) {maxpr = container_of(scroll,pcb_t,s_next)->priority;}
					scroll = scroll->next;
				}
			scroll = head->next;
			while (!list_is_last(head,scroll)){ /*scazza se il primo elemento è quello da inserire nel processore*/
				if(container_of(scroll,pcb_t,s_next)->priority == maxpr){
				LDST(container_of(scroll,pcb_t,s_next)->&(p_s));
				del = scroll;
				/*scroll = scroll->prev;*/
				current = container_of(del,pcb_t,s_next); /*aggiorna la var globale per davolz*/
				list_del(del);					
				}
			scroll = scroll->next;
			}
			scroll = head->next;
			while (!list_is_last(head,scroll)){container_of(scroll,pcb_t,s_next)->priority = container_of(scroll,pcb_t,s_next)->priority+1;}	
	setTIMER(3);
	}
else
	{scheduler_init(head);}


}

void scheduler_init(list_head* head){
		struct list_head* scroll = head->next;
		struct list_head* del;
		int maxpr = -1;
		while (!list_is_last(head,scroll)){
			if (container_of(scroll,pcb_t,s_next)->priority > maxpr){maxpr = container_of(scroll,pcb_t,s_next)->priority;}	
			scroll = scroll->next;
		}
		scroll = head->next;
		while (!list_is_last(scroll, head)){
			if (container_of(scroll,pcb_t,s_next)->priority = maxpr){
			LDST(container_of(scroll,pcb_t,s_next)->&(p_s));
			del = scroll;
			current = container_of(del,pcb_t,s_next);
				scroll = scroll->prev;
				list_del(struct list_head *del)}
		}
	setTIMER(3);

}
