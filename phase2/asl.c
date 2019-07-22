#include "const.h"
#include "listx.h"
#include "types_rikaya.h"
#include "asl.h"
#include "pcb.h"

static semd_t semd_table[MAXPROC];
static semd_t semdFree_h;
static semd_t semd_h;


/*DESCRIZIONE: Inizializza la lista dei semdFree in modo da contenere tutti gli elementi della semdTable.
Questo metodo viene invocato una volta sola durante l’inizializzazione della struttura dati.*/

/*Inizializzo le list_head s_next di semdFree_h e semd_h, successivamente aggiungo in coda gli elementi del vettore semd_table nella semdFree*/

void initASL(){
	int i;
	INIT_LIST_HEAD(&(semdFree_h.s_next));
	INIT_LIST_HEAD(&(semd_h.s_next));
	for (i=0; i<MAXPROC; i++){
		list_add_tail(&(semd_table[i].s_next), &(semdFree_h.s_next));
	}
}


/*Cerca nella ASL un semd con un determinato key. Se esiste restituisce un puntatore al semd, altrimenti NULL*/


semd_t* getSemd(int *key){
	struct list_head* tmp = &(semd_h.s_next);
	list_for_each(tmp, &(semd_h.s_next)){
		if (container_of(tmp, semd_t, s_next)->s_key == key)
			return container_of(tmp, semd_t, s_next);
	}
	return NULL;
}



/*Inserisco p nella coda dei processi bloccati associata al semd con chiave key. Se nella ASL non esiste un semd con quella chiave, allora
  ne aggiungo uno (prendendolo da semdFree) e setto la sua chiave a key, aggiungendo successivamente p alla coda dei processi bloccati
  Se non e' possibile allocare un nuovo semd perche' semdFree e' vuoto, ritorna true, altrimenti false*/


int insertBlocked(int *key,pcb_t *p){
	semd_t *tmp = getSemd(key);
	if (tmp != NULL) insertProcQ(&(tmp->s_procQ), p);
	else {
		if (list_empty(&(semdFree_h.s_next))) return 1;
		else {
			struct list_head* t = list_next(&(semdFree_h.s_next));
			list_del(t);
			list_add(t, &(semd_h.s_next));
			container_of(t, semd_t, s_next)->s_key = key;
			INIT_LIST_HEAD(&(container_of(t, semd_t, s_next)->s_procQ)); /*Per ogni semd aggiunto alla ASL uso questa macro per inizializzare la coda dei processi*/
			insertProcQ(&(container_of(t, semd_t, s_next)->s_procQ), p);
			p->p_semkey = key;
		}
	}
	return 0;
}



/*Rimuove il primo pcb dalla coda dei processi del semd (che deve essere nella ASL) con una determinata chiave key.
  Se non esiste un semd con tale key nella ASL ritorna NULL, altrimenti ritorna l'elemento rimosso.
  Se la coda dei processi bloccati diventa vuota, rimuove tale semd dalla ASL e lo aggiunge alla semdFree*/



pcb_t* removeBlocked(int *key){
	semd_t *tmp = getSemd(key);
	if (tmp == NULL) return NULL;
	else {
		pcb_t *pcb = removeProcQ(&(tmp->s_procQ));
		if (list_empty(&(tmp->s_procQ))){
			struct list_head *s = &(tmp->s_next);
			list_del(s);
			list_add_tail(s, &(semdFree_h.s_next));
		}
		return pcb;
	}
}



/*Rimuove p dalla coda dei processi del semaforo su cui e' bloccato (la chiave del semaforo sara' p->p_semkey)
  Se p non si trova nella coda, ritorna NULL, altrimenti p*/



pcb_t* outBlocked(pcb_t *p){
	struct semd_t *tmp = getSemd(p->p_semkey);
	if (tmp == NULL) return NULL;
	else{
		struct list_head *list = &(tmp->s_procQ);
		list_for_each(list, &(tmp->s_procQ)){
			if (container_of(list, pcb_t, p_next) == p)
			{
				list_del(&(p->p_next));
				if (list_empty(&(tmp->s_procQ))){
					list_del(&(tmp->s_procQ));
					list_add_tail(&(tmp->s_procQ), &(semdFree_h.s_next));
				}
				return p;
			}
		}
		return NULL;
	}
}



/*Ritorna, senza rimuovere, il puntatore al pcb che si trova in testa alla coda dei processi del semd con chiave key.
  Se il semd non compare nella ASL oppure la sua coda dei processi e' vuota, ritorna NULL*/


pcb_t* headBlocked(int *key){
	struct semd_t *tmp = getSemd(key);
	if (tmp == NULL || list_empty(&(tmp->s_procQ))) return NULL;
	else return container_of(list_next(&(tmp->s_procQ)), pcb_t, p_next);
}



/*Funzione ricorsiva che rimuove p (usando outBlocked) dalla coda dei processi del semaforo su cui e' bloccato.
  Successivamente, richiamo la funzione outChildBlocked su tutti i figli, rimuovendo cosi' tutto l'albero radicato in p*/



void outChildBlocked(pcb_t *p){
	struct list_head *tmp = &(p->p_child);
	outBlocked(p);
	list_for_each(tmp, &(p->p_child)){
		outChildBlocked(container_of(tmp, pcb_t, p_sib));
	}
}

