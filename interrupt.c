/*
    Copyright (C) 2019 Davide Davoli, Simone Branchetti, Michele Luca Contalbo

    This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License$

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General$

    You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>
*/


#include "types_rikaya.h"
#include "const.h"
#include "utils.h"
#include "interrupt.h"
#include "scheduler.h"
#include <umps/libumps.h>

extern pcb_t* current;
extern pcb_t ready_queue;

/*int rcvPLT(){
        unsigned int tmp = getCAUSE();
	tmp = tmp >> 8;
	return getBit(1,tmp);
}*/

/*Ritorna la posizione del pending interrupt. Visto che la ricerca di questo interrupt avviene dal primo all'ultimo, viene sempre preso l'interrupt con priorità più alta*/

int getDevice(unsigned int inst_dev, unsigned int int_dev){
	int i;
	for (i=0;i<8;i++)
		if(getBit(i,inst_dev) && getBit(i,int_dev))
			return i;
	return -1;
}

int getLineInt(){
	int i;
	unsigned int tmp = getCAUSE();
	for (i=IP; i<16; i++)
		if (getBit(i,tmp)) return i;
	return -1;
}


extern void int_handler(){
	int i;
	devreg_t* dev_register;
	copyState((state_t *)INT_OLD_AREA, &(current->p_s)); /*funzione definita in utils.c, copia lo stato dell'old area e lo mette in current, che è il puntatore all'ultimo pcb scelto dallo scheduler*/
	int line = getLineInt();
	if (line == IL_IPI+8){

	}
	else if (line == IL_CPUTIMER+8){
		setTIMER((unsigned int)-1); /*ack del Processor Local Timer*/
		scheduler(&(ready_queue.p_next));
	}
        else if (line == IL_TIMER+8){
		unsigned int *tmp = INT_TIMER;
		*tmp = (unsigned int)-1;
        }
	else if (line == IL_DISK+8){
		i = getDevice(INST_INT_LINE3, INT_DEV_LINE3);
		dev_register->(dtpreg *)dtp = (dtpreg_t *) DEV_REG_ADDR(IL_DISK, i); //nota: forse c'è errore su uso puntatore
		dev_register->dtp->command = CMD_ACK;
	}
        else if (line == IL_TAPE+8){
		i = getDevice(INST_INT_LINE4, INT_DEV_LINE4);
		dev_register->(dtpreg *)dtp = (dtpreg_t *) DEV_REG_ADDR(IL_TAPE, i);
		dev_register->dtp->command = CMD_ACK;
        }
        else if (line == IL_ETHERNET+8){
                i = getDevice(INST_INT_LINE5, INT_DEV_LINE5);
                dev_register->(dtpreg *)dtp = (dtpreg_t *) DEV_REG_ADDR(IL_ETHERNET, i);
                dev_register->dtp->command = CMD_ACK;
        }
        else if (line == IL_PRINTER+8){
                i = getDevice(INST_INT_LINE6, INT_DEV_LINE6);
                dev_register->(dtpreg *)dtp = (dtpreg_t *) DEV_REG_ADDR(IL_PRINTER, i);
                dev_register->dtp->command = CMD_ACK;
        }
        else if (line == IL_TERMINAL+8){
                i = getDevice(INST_INT_LINE7, INT_DEV_LINE7);
                dev_register->(dtpreg *)dtp = (dtpreg_t *) DEV_REG_ADDR(IL_TERMINAL, i);
                dev_register->dtp->command = CMD_ACK;
        }

}
