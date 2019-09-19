#ifndef _TYPES11_H
#define _TYPES11_H

#include <umps/types.h>
#include <listx.h>

typedef unsigned int memaddr;

// Process Control Block (PCB) data structure
typedef struct pcb_t {
	/*process queue fields */
	struct list_head	p_next;

	/*process tree fields */
	struct pcb_t		*p_parent;
	struct list_head	p_child, p_sib;

	/* processor state, etc */
	state_t       		p_s;

	/* process priority */
	int			priority;
	/*original priority*/
	int			original_priority;

	/* key of the semaphore on which the process is eventually blocked */
	int			*p_semkey;

	/*TUTTI I TEMPI SONO ESPRESSI IN CICLI DI CLOCK*/

	int			initial_time;  /*tempo di inizio del processo*/
	int 		last_syscall_time; /*tempo di chiamata dell' ultima syscall. serve a controllare quanto tempo ci passo dentro*/
	int			middle_time; /*tempo di uscita dell' ultima syscall eseguita dal programma. calcola con questo il tempo in user mode alla prossima syscall*/
	int			total_time_user; /*tempo totale passato in user mode*/
	int			total_time_kernel; /*tempo totale passato in kernel mode*/


	int tutor;


	state_t *sysbk_old, *tlb_old, *pgmtrap_old; /*areas for various handlers*/
	state_t *sysbk_new, *tlb_new, *pgmtrap_new;
	short int spec_assigned[3];

} pcb_t;



// Semaphore Descriptor (SEMD) data structure
typedef struct semd_t {
	struct list_head	s_next;

	// Semaphore key
	int    *s_key;

	// Queue of PCBs blocked on the semaphore
	struct list_head	s_procQ;
} semd_t;

#define STATE_GPR_LEN 29

/* Processor state
typedef struct state {
    unsigned int entry_hi;
    unsigned int cause;
    unsigned int status;
    unsigned int pc_epc;
    unsigned int gpr[STATE_GPR_LEN];
    unsigned int hi;
    unsigned int lo;
} state_t;*/

#define reg_at  gpr[0]
#define reg_v0  gpr[1]
#define reg_v1  gpr[2]
#define reg_a0  gpr[3]
#define reg_a1  gpr[4]
#define reg_a2  gpr[5]
#define reg_a3  gpr[6]
#define reg_t0  gpr[7]
#define reg_t1  gpr[8]
#define reg_t2  gpr[9]
#define reg_t3  gpr[10]
#define reg_t4  gpr[11]
#define reg_t5  gpr[12]
#define reg_t6  gpr[13]
#define reg_t7  gpr[14]
#define reg_s0  gpr[15]
#define reg_s1  gpr[16]
#define reg_s2  gpr[17]
#define reg_s3  gpr[18]
#define reg_s4  gpr[19]
#define reg_s5  gpr[20]
#define reg_s6  gpr[21]
#define reg_s7  gpr[22]
#define reg_t8  gpr[23]
#define reg_t9  gpr[24]
#define reg_gp  gpr[25]
#define reg_sp  gpr[26]
#define reg_fp  gpr[27]
#define reg_ra  gpr[28]
#define reg_HI  gpr[29]
#define reg_LO gpr[30]

#endif
