#include "utils.h"
#include "types_rikaya.h"

typedef char word;

extern void addokbuf();

int getBit (int shift, unsigned int reg){
        if (shift < 0 || shift >31) return -1;
        unsigned int tmp = 1;
        tmp = tmp << shift;
        return (tmp & reg) >> shift;
}

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

void copyState(state_t* src, state_t* dst){
	int i=0;
	word *reader, *writer;
	for (i=0,reader=(word*) src, writer=(word*) dst; i<sizeof(state_t);i++, reader++, writer++)
		*writer=*reader;

}

void tlb_handler(){return;}
void trap_handler(){addokbuf("trap handler");}
