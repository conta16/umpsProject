#ifndef UTILS_H
#define UTILS_H

#include <umps/libumps.h>


#define INT_NEW_AREA 0x2000008C
#define TLB_NEW_AREA 0x200001A4
#define TRAP_NEW_AREA 0x200002BC
#define SYSCALL_NEW_AREA 0x200003D4

#define INT_OLD_AREA 0x20000000
#define TLB_OLD_AREA 0x20000118
#define TRAP_OLD_AREA 0x20000230
#define SYSCALL_OLD_AREA 0x20000348

#define IEc 0
#define KUc 1
#define IEp 2
#define KUp 3
#define IEo 4
#define KUo 5
#define IM  8    /*Interrupt Mask: 8 is only the first bit, it goes on till 15 depending on the type of interrupt*/
#define BEV 22
#define VMc 24
#define VMp 25
#define VMo 26
#define TE  27
#define CU  28   /*Coprocessor Usability: 28 is only the first bit, it goes on till 31th bit*/


void tlb_handler(); /*Declaration of handling functions, to be moved in the correct library, when implemented*/
void trap_handler();


int getBit (int shift, unsigned int reg);
int setBit();
unsigned int getExcCode();

#endif
