/*
    Copyright (C) 2019 Davide Davoli, Simone Branchetti, Michele Luca Contalbo

    This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License$

    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General$

    You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>
*/

#include "utils.h"
#include "types_rikaya.h"
#include "umps/libumps.h"
#include "umps/arch.h"
#include "umps/types.h"
#include "const_rikaya.h"
typedef char word;


#define ST_READY           1
#define ST_BUSY            3
#define ST_TRANSMITTED     5

#define CMD_ACK            1
#define CMD_TRANSMIT       2

void wait(){
	while(1);
}

/*input:
	shift: indica la posizione del bit che si vuole ottenere
	reg: è l'indirizzo del registro di cui si vuole ottenere il shift-esimo bit
output:
	-1 se i valori di shift sono sbagliati
	0 o 1 a seconda del valore del shift-esimo bit
*/

int getBit (int shift, unsigned int reg){
        if (shift < 0 || shift >31) return -1;
        unsigned int tmp = 1;
        tmp = tmp << shift;
        return (tmp & reg) >> shift;
}

int get_bit(int shift, unsigned int reg){
	if (shift < 0 || shift > 31) return -1;
	unsigned int tmp = 1;
	tmp = tmp << shift;
	if ((tmp & reg) >> shift == (unsigned int) 1) return 1;
	else return 0;
}
/*
input:
	shift: analogo a sopra
	reg: analogo a sopra
	value: il valore a cui si vuole settare il shift-esimo bit

output:
	-1 in caso di errore su shift oppure su value
	0 nel caso in cui l'operazione sia stata eseguita con successo

Visto che viene passato reg come puntatore, il nuovo registro con il value settato sarà disponibile anche dalla funzione chiamante
*/


int setBit (int shift, unsigned int *reg, int value){
        if (shift < 0 || shift > 31 || value < 0 || value > 1) return -1;
        unsigned int tmp = 1;
        tmp = tmp << shift;
        if (value == 1) *reg = *reg | tmp;
        else if (value == 0){
                tmp = ~tmp;
                *reg = *reg & tmp;
        }
        return 0;
}


unsigned int getExcCode(){
        unsigned int cause=getCAUSE();
        return (cause&124)>>2;
}

/*copia byte per byte src in dst*/

void copyState(state_t* src, state_t* dst){
	int i=0;
	word *reader, *writer;
	for (i=0,reader=(word*) src, writer=(word*) dst; i<sizeof(state_t);i++, reader++, writer++)
		*writer=*reader;

}

unsigned int getClock(){
  long int clock;
  clock = *((unsigned int*)(CLOCK_HIGH));
  return clock;  /*return getTODLO();*/
}

void disable_interrupts(){
	setSTATUS(getSTATUS()&(~1UL));
}

void enable_interrupts(){
	setSTATUS(getSTATUS()|(1UL));
}
