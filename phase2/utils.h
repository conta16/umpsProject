#ifndef UTILS_H
#define UTILS_H

#include <umps/libumps.h>
#include "types_rikaya.h"

#define CMD_ACK 1

#define IEc 0
#define KUc 1
#define IEp 2
#define KUp 3
#define IEo 4
#define KUo 5
#define IM  8    /*Interrupt Mask: 8 is only the first bit, it goes on till 15 depending on the type of interrupt*/
#define IP IM
#define BEV 22
#define VMc 24
#define VMp 25
#define VMo 26
#define TE  27
#define CU  28   /*Coprocessor Usability: 28 is only the first bit, it goes on till 31th bit*/

#define INST_INT_LINE3 0x10000028
#define INST_INT_LINE4 0x1000002C
#define INST_INT_LINE5 0x10000030
#define INST_INT_LINE6 0x10000034
#define INST_INT_LINE7 0x10000038

#define INT_DEV_LINE3 0x1000003C
#define INT_DEV_LINE4 0x10000040
#define INT_DEV_LINE5 0x10000044
#define INT_DEV_LINE6 0x10000048
#define INT_DEV_LINE7 0x1000004C

#define INBOX 0x10000400
#define OUTBOX 0x10000404

#define IL_IPI 0
#define IL_CPUTIMER 1
#define IL_TIMER 2
#define IL_DISK 3
#define IL_TAPE 4
#define IL_ETHERNET 5
#define IL_PRINTER 6
#define IL_TERMINAL 7

#define DEV_NOT_INST 0
#define DEV_READY 1
#define ILLEGAL_OP 2
#define DEV_BUSY 3
#define RECV_ERROR 4
#define TRANSM_ERROR 4
#define CHAR_RECVD 5
#define CHAR_TRANSMD 5
#define CMD_RESET 0
#define CMD_ACK 1
#define CMD_TRANSM 2
#define CMD_RECV 2
#define I_TIMER 0x10000020

#define CLOCK_HIGH 0x10000018
#define CLOCK_LOW  0x1000001c

typedef unsigned int u32;

void copyState (state_t *src, state_t *dst);
int getBit (int shift, unsigned int reg);
int setBit();
unsigned int getExcCode();
unsigned int getClock();
void term_puts(const char *str);
int term_putchar(char c);
u32 tx_status(termreg_t *tp);

#endif
