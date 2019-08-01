/*
    Copyright (C) 2019 Davide Davoli, Simone Branchetti, Michele Luca Contalbo

    This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License$

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General$

    You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>
*/


#include "types_rikaya.h"
#include "const_rikaya.h"
#include "const.h"
#include "utils.h"
#include "interrupt.h"
#include "scheduler.h"
#include "arch.h"
#include "asl.h"
#include <umps/libumps.h>

extern pcb_t* current;
extern pcb_t ready_queue;
extern unsigned int keys[49];
int rcvPLT(){
        unsigned int tmp = getCAUSE();
	tmp = tmp >> 8;
	return getBit(1,tmp);
}


int getDevice(unsigned int inst_dev, unsigned int int_dev){
	int i;
	for (i=0;i<8;i++)
		if(getBit(i,inst_dev) && getBit(i,int_dev))
			return i;
	return -1;
}

/*Ritorna la posizione del pending interrupt. Visto che la ricerca di questo interrupt avviene dal primo all'ultimo, viene sempre preso l'interrupt con priorità più alta*/

int getLineInt(){
	int i;
	unsigned int tmp = getCAUSE();
	for (i=IP; i<16; i++)
		if (getBit(i,tmp)) return i;
	return -1;
}


extern void int_handler(){
	int i;
	dtpreg_t* dev_register;
	termreg_t* term_register;
	if (current != NULL) copyState((state_t *)INT_OLDAREA, &(current->p_s)); /*funzione definita in utils.c, copia lo stato dell'old area e lo mette in current, che è il puntatore all'ultimo pcb scelto dallo scheduler*/
	int line = getLineInt();
	if (line == IL_IPI+8){
		/*nessuna azione significativa è associata a questa linea per questa fase: mi limito a mandare l'ack, senza sapere quale processore ha mandato l'interrupt*/
		unsigned int *tmp = (unsigned int*) INBOX;
		*tmp = (unsigned int)-1;
	}
	else if (line == IL_CPUTIMER+8){
		setTIMER((unsigned int)-1); /*ack del Processor Local Timer*/
		scheduler(&(ready_queue.p_next));
	}
        else if (line == IL_TIMER+8){
		unsigned int *tmp = (unsigned int *)I_TIMER;
                *tmp = (unsigned int)-1;
                while(getSemd((int *)&(keys[48])) != NULL)
                        SYSCALL(VERHOGEN,(int)&(keys[48]),0,0);
        }
	else if (line == IL_DISK+8){
		i = getDevice(INST_INT_LINE3,INT_DEV_LINE3);
		dev_register = (dtpreg_t *) DEV_REG_ADDR(IL_DISK, i);
		dev_register->command = CMD_ACK;
		SYSCALL(VERHOGEN,(int)&(keys[i]),0,0);
	}
        else if (line == IL_TAPE+8){
		i = getDevice(INST_INT_LINE4,INT_DEV_LINE4);
		dev_register = (dtpreg_t *) DEV_REG_ADDR(IL_TAPE, i);
		dev_register->command = CMD_ACK;
		SYSCALL(VERHOGEN,(int)&(keys[8+i]),0,0);
        }
        else if (line == IL_ETHERNET+8){
                i = getDevice(INST_INT_LINE5,INT_DEV_LINE5);
                dev_register = (dtpreg_t *) DEV_REG_ADDR(IL_ETHERNET, i);
                dev_register->command = CMD_ACK;
		SYSCALL(VERHOGEN,(int)&(keys[16+i]),0,0);
        }
        else if (line == IL_PRINTER+8){
                i = getDevice(INST_INT_LINE6,INT_DEV_LINE6);
		dev_register = (dtpreg_t *) DEV_REG_ADDR(IL_PRINTER, i);
                dev_register->command = CMD_ACK;
		SYSCALL(VERHOGEN,(int)&(keys[24+i]),0,0);
        }
        else if (line == IL_TERMINAL+8){
                void wait(){
                        while (term_register->transm_status == DEV_BUSY);
                }
                i = getDevice(INST_INT_LINE7,INT_DEV_LINE7);
		if (i==3) wait();
                term_register = (termreg_t *) DEV_REG_ADDR(IL_TERMINAL, 0);/*da mettere +i*/
                if ((term_register->transm_status & (unsigned int)255) == CHAR_TRANSMD){
			wait();
			wait();
			term_register->transm_command = CMD_ACK;
			SYSCALL(VERHOGEN,(int)&(keys[32]),0,0);/*da mettere +i*/
		}
		if ((term_register->recv_status & (unsigned int)255) == CHAR_RECVD){
			wait();
			term_register->recv_command = CMD_ACK;
			SYSCALL(VERHOGEN,(int)&(keys[40]),0,0);/*da mettere +i*/
		}
        }

}
