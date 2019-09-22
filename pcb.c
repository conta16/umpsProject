#include "const.h"
#include "listx.h"
#include "types_rikaya.h"
#include "pcb.h"

static pcb_t pcbFree_table[MAXPROC];
static pcb_t pcbfree_h;
struct list_head* pcbFree=&(pcbfree_h.p_next);

/* Inizializziamo pcbfree_h e la list_head ivi contenuta nel campo p_next. Nel ciclo vengono inseriti gli elementi di pcbFree_table nella lista pcbFree*/

void initPcbs(void) {
	int i;
	INIT_LIST_HEAD(&(pcbfree_h.p_next));
	for (i=0; i<MAXPROC; i++){
		INIT_LIST_HEAD(&(pcbFree_table[i].p_child));
		list_add(&pcbFree_table[i].p_next, pcbFree);
	}
}

/*Semplice incapsulamento della macro list_add: liberare un pcb coincide con aggiungerlo alla lista dei pcb liberi*/

void freePcb(pcb_t * p) {
	list_add(&(p->p_next), pcbFree);
}

/*Con la macro container_of si ottiene il primo pcb della lista pcbFree, lo si alloca cancellandolo dalla lista e restituendo il suo indirizzo con la macro list_del, mentre se la lista è vuota si restituisce NULL*/

pcb_t* allocPcb(void) {
	pcb_t* first=NULL;
	int i;    /* Conta il numero di byte già messi a 0 all' interno di p_s*/
	char* b;  /* Puntatore della dimensione di un byte che punta all'inizio di p_s, in modo da poterla inizializzare a 0 byte per byte */
	if (!list_empty(pcbFree)){
		first=container_of(pcbFree->next, pcb_t, p_next);
		list_del(list_next(pcbFree));
		INIT_LIST_HEAD(&(first->p_next)); /* Inizializzazione del pcb come da specifica: le liste vengono rese utilizzabili con INIT_LIST_HEAD e gli altri campi vengono messi a 0/NULL*/
		INIT_LIST_HEAD(&(first->p_child));
		INIT_LIST_HEAD(&(first->p_sib));
		first->p_parent=NULL;
		b=(char* )&(first->p_s);
		for (i=0; i<sizeof(state_t); i++){/* Ciclo di scorrimento per azzerare il campo p_s*/
			*b=0;
			b++;
			}
		first->priority=0;
		first->original_priority=0;
		first->p_semkey=NULL;
		first->initial_time=-1;
		first->middle_time=-1;
		first->total_time_user=0;
		first->total_time_kernel=0;
		first->tutor=0;
		first->sysbk_old=NULL; first->tlb_old=NULL; first->pgmtrap_old=NULL;
		first->sysbk_new=NULL; first->tlb_new=NULL; first->pgmtrap_new=NULL;
		for (int i=0; i<3; i++)
			first->spec_assigned[i] = 0;
	}
	return first;
}

/*Semplice incapsulamento della macro INIT_LIS_THEAD*/

void mkEmptyProcQ(struct list_head* head){
	INIT_LIST_HEAD(head);
}

/*Semplice incapsulamento della macro list_empty*/

int emptyProcQ(struct list_head* head){
	return list_empty(head);
}

/*La funzione corre la lista head, fino a raggiungere un elemento con priorità maggiore in senso lato di p, o fino all'ultimo elemento e qui si aggiumge p.*/
void insertProcQ(struct list_head* head, pcb_t * p){
	struct list_head* begin=head;
	while ((p->priority)<=(container_of(head->next, pcb_t, p_next)->priority) && !list_is_last(head, begin))
		head=head->next;
	list_add(&(p->p_next), head);
}

/*Con container_of() facciamo ritornare la testa in maniera semplice e concisa*/

pcb_t* headProcQ(struct list_head* head){
	return container_of(head->next, pcb_t, p_next);
}

/*Ci assicuriamo che la lista non sia vuota e poi rimuoviamo il primo elemento in caso non lo sia usato con list_del()*/

pcb_t* removeProcQ(struct list_head* head){
	pcb_t* del = NULL;
	if (list_empty(head))
		return NULL;
	else{
		del = container_of(list_next(head), pcb_t, p_next);
		list_del(list_next(head));
		return del;
	}

}

/*Per scorrere la lista usiamo un list_head* d' appoggio, request, che facciamo ritornare quando individuiamo l' elemento richiesto*/

pcb_t* outProcQ(struct list_head* head, pcb_t *p){
	struct list_head* request = NULL;
	struct list_head* cursor = head->next;
	if (list_empty(head))
		return NULL;
	else{
		while (cursor != head)
		{
			if (cursor == &(p->p_next)){
				request = cursor;
				list_del(cursor);
				return container_of(request, pcb_t, p_next);
			}
			cursor = cursor->next;
		}
	}
	return NULL;
}

/*Per poter usare la macro list_empty() dobbiamo appoggiarci ad una variabile tmp di tipo list_head*. Usiamo poi la macro list_empty() per restituire il valore richiesto*/

int emptyChild(pcb_t* p){
	struct list_head *tmp = &(p->p_child);
	return list_empty(tmp);
}

/*Aggiungiamo (in coda) alla list_head p_child di prnt la list_head p_sib di p.
Quindi, in questo modo mettiamo p come figlio di prnt*/

void insertChild (pcb_t* prnt, pcb_t* p){
	p->p_parent = prnt;
	list_del(&(p->p_sib));
	list_add_tail(&(p->p_sib), &(prnt->p_child));
}



/*Usiamo la funzione outChild() per eliminare il primo figlio passandolo come parametro*/

pcb_t* removeChild (pcb_t* p){
	if (!list_empty(&(p->p_child)))
		return outChild(container_of(p->p_child.next, pcb_t, p_sib));
	return NULL;
}

/*Rimuoviamo p dalla lista dei figli del padre (p puo' avere una posizione qualsiasi all'interno della lista).
  Se p non ha un padre, ritorna NULL*/

pcb_t* outChild (pcb_t* p){
	if (p->p_parent != NULL){
		list_del(&(p->p_sib));
		p->p_parent = NULL;
		return p;
	}
	return NULL;
}
