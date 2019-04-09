#ifndef UTILS_H
#define UTILS_H

#define RAMTOP 0xFFFFFFFF

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


#endif
