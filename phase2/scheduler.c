/*
    Copyright (C) 2019 Davide Davoli, Simone Branchetti, Michele Luca Contalbo

    This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License$

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General$

    You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>
*/

#include "scheduler.h"
#include "listx.h"
#include "types_rikaya.h"
#include "utils.h"
#include <umps/libumps.h>
#include "pcb.h"
#include "const_rikaya.h"

#define SEMAPHORE S32

extern pcb_t* idle;
extern pcb_t ready_queue;
pcb_t* current;

void scheduler(struct list_head* head){
	if (current==idle) current=NULL;
	if (current!=NULL){
		current->priority = current->original_priority;/*se il processore ha un processo ne riaggiorniamo la priorità, settandola a quella originale*/
		increment_pcbs_priority(head);/*effettuiamo l' aging su tutti gli altri processi*/
		insertProcQ(head,current);/*reinseriamo il processo corrente nella coda dei processi*/
		current=removeProcQ(head);/*scegliamo un altro processo da eseguire usando le funzioni di pcb.c*/
		setTIMER(3000*TIME_SCALE);/*aggiorniamo il time-slice*/
		LDST(&(current->p_s));/*carichiamo il processo nel processore*/
	}
	else{
		/*se il processore non ha un processo significa che il processo prima ha terminato la sua esecuzione
		esattamente alla fine dell' ultimo time-slice quindi lo trattiamo come una nuova inizializzazione*/
		if (!list_empty(&(ready_queue.p_next))){
			scheduler_init(head);
		}
		else{
			current = idle;
			setTIMER(3000*TIME_SCALE);
			setSTATUS(getSTATUS() & (unsigned int)(~(1)));
			LDST(&(current->p_s));
		}
	}
}

void scheduler_init(struct list_head* head){
	/*questa funzione seleziona il processo con priorità maggiore e lo inserisce nel processore facendo l' aging degli altri nel frattempo*/
	current=removeProcQ(head);
	if (current->initial_time != -1)
		current->initial_time = getClock();
	if (current->middle_time != -1)
		current->middle_time = current->initial_time;
	increment_pcbs_priority(head);
	setTIMER(3000*TIME_SCALE);
	//setSTATUS(getSTATUS()|1);/*impostiamo lo stato del processore*/
	LDST(&(current->p_s));
}

void increment_pcbs_priority(struct list_head* head){
	if (head != NULL){
	/*scorriamo la lista e incrementiamo la priorità di ogni processo presente (che quindi non è nel processore)*/
		struct list_head* scroll = head->next;
		list_for_each(scroll,&(ready_queue.p_next)){
			container_of(scroll,pcb_t,p_next)->priority = container_of(scroll,pcb_t,p_next)->priority+1;
		}
	}
}
