#ifndef TAPEUTILS_H
#define TAPEUTILS_H

#include "tapearch.h"
#include "umps/types.h"
#include "umps/arch.h"
#include "strutils.h"

#define BLOCK_SIZE 4096
#define EOT 0
#define EOF 1
#define EOB 2
#define TS 3

/*status tape*/

#define DEV_NOT_INST 0
#define DEV_READY 1
#define ILLEG_OP 2
#define DEV_BUSY 3
#define SKIP_ERR 4
#define READ_ERR 5
#define BACK_BLOCK_ERR 6
#define DMA_TRANSF_ERR 7

/*commands tape*/

#define RESET 0
#define ACK 1
#define SKIPBLK 2
#define READBLK 3
#define BACKBLK 4

/*transmission terminal status*/

#define ST_DEV_NOT_INST    0
#define ST_READY           1
#define ST_ILLEG_OP        2
#define ST_BUSY            3
#define ST_TRANSM_ERR      4
#define ST_TRANSMITTED     5

/*receive terminal status: <4 are the same*/

#define ST_RCV_ERR         4
#define ST_RECEIVED        5

/*transmission terminal command*/

#define CMD_RESET          0
#define CMD_ACK            1
#define CMD_TRANSMIT       2

/*receive terminal status (0 and 1 are the same)*/

#define CMD_RECEIVE        2



#define CHAR_OFFSET        8
#define TERM_STATUS_MASK   0xFF

static termreg_t *term0_reg = (termreg_t *) DEV_REG_ADDR(IL_TERMINAL, 0);
typedef unsigned int u32;
typedef dtpreg_t* tape_reg;

static u32 tx_status(termreg_t *tp)
{
    return ((tp->transm_status) & TERM_STATUS_MASK);
}


/*useful for debugging, it prints an integer called from printInt*/

static int printToTerm(int rest)
{
    char c;

    u32 stat;

    stat = tx_status(term0_reg);
    if (stat != ST_READY && stat != ST_TRANSMITTED)
        return -1;

    switch(rest){
        case 0: term0_reg->transm_command = ((48 << CHAR_OFFSET) | CMD_TRANSMIT);
        case 1: term0_reg->transm_command = ((49 << CHAR_OFFSET) | CMD_TRANSMIT);
        case 2: term0_reg->transm_command = ((50 << CHAR_OFFSET) | CMD_TRANSMIT);
        case 3: term0_reg->transm_command = ((51 << CHAR_OFFSET) | CMD_TRANSMIT);
        case 4: term0_reg->transm_command = ((52 << CHAR_OFFSET) | CMD_TRANSMIT);
        case 5: term0_reg->transm_command = ((53 << CHAR_OFFSET) | CMD_TRANSMIT);
        case 6: term0_reg->transm_command = ((54 << CHAR_OFFSET) | CMD_TRANSMIT);
        case 7: term0_reg->transm_command = ((55 << CHAR_OFFSET) | CMD_TRANSMIT);
        case 8: term0_reg->transm_command = ((56 << CHAR_OFFSET) | CMD_TRANSMIT);
        default: term0_reg->transm_command = ((47 << CHAR_OFFSET) | CMD_TRANSMIT);
        }

    while ((stat = tx_status(term0_reg)) == ST_BUSY)
        ;

    term0_reg->transm_command = CMD_ACK;
 if (stat != ST_TRANSMITTED)
        return -1;
    else
        return 0;

}


static void printInt(int a)
{
        if (a==0) return;
        int rest;
        printInt(a/10);
        rest = a%10;
        if(printToTerm(rest)) return;
}

static int loadblock(dtpreg_t *tape_reg, unsigned long int  ramaddr);


static int chooseTape(){
    int indexTape;
    term_puts("Which file do you want to open?\n");
    do{
        indexTape = term_getchar();
	if (indexTape-48 <=0 || indexTape-48 > 8) term_puts("Invalid number, retry\n");
    }while(indexTape-48 <=0 || indexTape-48 > 8);
    return indexTape;
}



static int loadblock(dtpreg_t *tape_reg, unsigned long int  ramaddr){

    unsigned int status;

    while ((status=tape_reg->status)==TAPE_BUSY || (status=tape_reg->status)==TAPE_NOTAPE)
        ;
    tape_reg->data0=ramaddr;
    tape_reg->command=TAPE_READBLK;
    while ((status=tape_reg->status)==TAPE_BUSY)
        ;
    if (status!=TAPE_READY)
         return -1;
    tape_reg->command=TAPE_ACK;
    return tape_reg->data1;
}

static int load_file(dtpreg_t *tape_reg, unsigned long int  ramaddr){
    int n;
    while ((n=loadblock(tape_reg, ramaddr))!=EOF && n>0){
        ramaddr+=4096;
        loadblock(tape_reg, ramaddr);
            }
    return ramaddr;
}
#endif
