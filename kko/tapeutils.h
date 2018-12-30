#ifndef TAPEUTILS_H
#define TAPEUTILS_H

#include "tapearch.h"
#include "umps/types.h"
#include "umps/arch.h"
#include "strutils.h"

#define BLOCK_SIZE 4096

typedef unsigned int u32;
typedef dtpreg_t* tape_reg;


static int loadblock(dtpreg_t *tape_reg, char*  ramaddr);

static int loadblock(dtpreg_t *tape_reg, char*  ramaddr){

    unsigned int status;

    while ((status=tape_reg->status)==TAPE_BUSY || (status=tape_reg->status)==TAPE_NOTAPE) /*Wait for tape to be ready*/
        ;
    tape_reg->data0=(unsigned int)ramaddr; /*Set the memory address where the block will be saved*/
    tape_reg->command=TAPE_READBLK; /* Send the command to the device */
    while ((status=tape_reg->status)==TAPE_BUSY) /*Wait for tape to finish the operation */
        ;
    if (status!=TAPE_READY) /*in case of error return -1*/
         return -1;
    tape_reg->command=TAPE_ACK; /*Send ack to the tape device */
    return tape_reg->data1; /*Return the last tape marker*/
}
#endif
