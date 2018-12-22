#include "umps/libumps.h"
#include "umps/arch.h"
#include "umps/types.h"
#include "strutils.h"
#include "terminal.h"
#include "tapeutils.h"

#define ST_READY           1
#define ST_BUSY            3
#define ST_TRANSMITTED     5

#define CMD_ACK            1
#define CMD_TRANSMIT       2

#define CHAR_OFFSET        8
#define TERM_STATUS_MASK   0xFF

#define RAM_BEGIN          0x20007000
#define BLOCK_SIZE         4096

#define STD_TERM_W 72
#define STD_TERM_H 24

typedef unsigned int u32;


/*the 8 registers that point to the tape devices + endOfRAM which is a pointer to the last written word in the RAM*/

dtpreg_t *tape0_register;
dtpreg_t *tape1_register;
dtpreg_t *tape2_register;
dtpreg_t *tape3_register;
dtpreg_t *tape4_register;
dtpreg_t *tape5_register;
dtpreg_t *tape6_register;
dtpreg_t *tape7_register;
dtpreg_t *endOfRAM;
static int load_ram_block(char *addr);
static int display_window(int posSpace);
static void init_window();
static char bbuffer[BLOCK_SIZE];
static void term_readline();
static int ld_block();

/*this struct is used to prevent the last word of the block to be cut. That word will be entirely displayed in the next block*/

static struct tmp{
char tmp_c[128];
int tmp_len;
}tmp_t;
static struct window{
    int bufpos;
	} finestra;

int lavallo;
int indexTape;


static void loadTapess ()
{
    /*I tell where the tape pointers will point*/
    int stat;
    int i=0;
    tape0_register = (dtpreg_t *) DEV_REG_ADDR(IL_TAPE, 0);
    tape0_register->data0 = RAM_BEGIN;
    tape1_register = (dtpreg_t *) DEV_REG_ADDR(IL_TAPE, 1);
    tape1_register->data0 = RAM_BEGIN;
    tape2_register = (dtpreg_t *) DEV_REG_ADDR(IL_TAPE, 2);
    tape2_register->data0 = RAM_BEGIN;
    tape3_register = (dtpreg_t *) DEV_REG_ADDR(IL_TAPE, 3);
    tape3_register->data0 = RAM_BEGIN;
    tape4_register = (dtpreg_t *) DEV_REG_ADDR(IL_TAPE, 4);
    tape4_register->data0 = RAM_BEGIN;
    tape5_register = (dtpreg_t *) DEV_REG_ADDR(IL_TAPE, 5);
    tape5_register->data0 = RAM_BEGIN;
    tape6_register = (dtpreg_t *) DEV_REG_ADDR(IL_TAPE, 6);
    tape6_register->data0 = RAM_BEGIN;
    tape7_register = (dtpreg_t *) DEV_REG_ADDR(IL_TAPE, 7);
    tape7_register->data0 = RAM_BEGIN;
    dtpreg_t *tmp_tape_register = tape0_register;


   /*Loading of the files in the RAM. I use tmp_tape_register to iterate in the RAM and, at the end of each do while, I define where tape[i]_register
     must point (beginning of his file) and I make tmp_tape_register point to the next tape*/


    tape0_register->data0 = tmp_tape_register->data0;

    do{
        stat = loadblock(tmp_tape_register,tmp_tape_register->data0);
	tmp_tape_register->data0 = tmp_tape_register->data0+BLOCK_SIZE;
    }while(stat != EOF && stat != EOT);

    tape1_register->data0 = tmp_tape_register->data0;
    tmp_tape_register = tape1_register;

    do{
        stat = loadblock(tmp_tape_register,tmp_tape_register->data0);
        tmp_tape_register->data0 = tmp_tape_register->data0+BLOCK_SIZE;
    }while(stat != EOF && stat != EOT);

    tape2_register->data0 = tmp_tape_register->data0;
    tmp_tape_register = tape2_register;

    do{
        stat = loadblock(tmp_tape_register,tmp_tape_register->data0);
        tmp_tape_register->data0 = tmp_tape_register->data0+BLOCK_SIZE;
    }while(stat != EOF && stat != EOT);

    tape3_register->data0 = tmp_tape_register->data0;
    tmp_tape_register = tape3_register;

    do{
        stat = loadblock(tmp_tape_register,tmp_tape_register->data0);
        tmp_tape_register->data0 = tmp_tape_register->data0+BLOCK_SIZE;
    }while(stat != EOF && stat != EOT);

    tape4_register->data0 = tmp_tape_register->data0;
    tmp_tape_register = tape4_register;

    do{
        stat = loadblock(tmp_tape_register,tmp_tape_register->data0);
        tmp_tape_register->data0 = tmp_tape_register->data0+BLOCK_SIZE;
    }while(stat != EOF && stat != EOT);

    tape5_register->data0 = tmp_tape_register->data0;
    tmp_tape_register = tape5_register;

    do{
        stat = loadblock(tmp_tape_register,tmp_tape_register->data0);
        tmp_tape_register->data0 = tmp_tape_register->data0+BLOCK_SIZE;
    }while(stat != EOF && stat != EOT);

    tape6_register->data0 = tmp_tape_register->data0;
    tmp_tape_register = tape6_register;

    do{
        stat = loadblock(tmp_tape_register,tmp_tape_register->data0);
        tmp_tape_register->data0 = tmp_tape_register->data0+BLOCK_SIZE;
    }while(stat != EOF && stat != EOT);

    tape7_register->data0 = tmp_tape_register->data0;
    tmp_tape_register = tape7_register;

    do{
        stat = loadblock(tmp_tape_register,tmp_tape_register->data0);
        tmp_tape_register->data0 = tmp_tape_register->data0+BLOCK_SIZE;
    }while(stat != EOF && stat != EOT);

    endOfRAM = tmp_tape_register;

}


void main(void)
{
    char a[128];
    int i=0;
    int posSpace;
    loadTapess();
    indexTape = chooseTape()-48; /*which tape will be seen*/
    do lavallo=ld_block(); while (lavallo);
    /* Go to sleep and power off the machine if anything wakes us up */
    WAIT();
    *((u32 *) MCTL_POWER) = 0x0FF;
    while (1) ;
}

static int ld_block(void){
    init_window();
    term_puts(tmp_t.tmp_c); /*see the word that was cut down*/
    int posSpace;
    char buf[128];

    /*load in the bbuffer the different blocks*/

    if (indexTape == 1) posSpace = load_ram_block(tape0_register->data0);
    else if (indexTape == 2) posSpace = load_ram_block(tape1_register->data0);
    else if (indexTape == 3) posSpace = load_ram_block(tape2_register->data0);
    else if (indexTape == 4) posSpace = load_ram_block(tape3_register->data0);
    else if (indexTape == 5) posSpace = load_ram_block(tape4_register->data0);
    else if (indexTape == 6) posSpace = load_ram_block(tape5_register->data0);
    else if (indexTape == 7) posSpace = load_ram_block(tape6_register->data0);
    else if (indexTape == 8) posSpace = load_ram_block(tape7_register->data0);
    display_window(posSpace); /*display block*/

    /*In this part, I make the pointer point to the next block, and if it's equal to the pointer of the next tape, then it returns 0*/
    if (indexTape == 1) if((tape0_register->data0 = tape0_register->data0+BLOCK_SIZE) >=tape1_register->data0) return 0;
    if (indexTape == 2) if((tape1_register->data0 = tape1_register->data0+BLOCK_SIZE) >= tape2_register->data0) return 0;
    if (indexTape == 3) if((tape2_register->data0 = tape2_register->data0+BLOCK_SIZE) >= tape3_register->data0) return 0;
    if (indexTape == 4) if((tape3_register->data0 = tape3_register->data0+BLOCK_SIZE) >= tape4_register->data0) return 0;
    if (indexTape == 5) if((tape4_register->data0 = tape4_register->data0+BLOCK_SIZE) >= tape5_register->data0) return 0;
    if (indexTape == 6) if((tape5_register->data0 = tape5_register->data0+BLOCK_SIZE) >= tape6_register->data0) return 0;
    if (indexTape == 7) if((tape6_register->data0 = tape6_register->data0+BLOCK_SIZE) >= tape7_register->data0) return 0;
    if (indexTape == 8) if((tape7_register->data0 = tape7_register->data0+BLOCK_SIZE) >= endOfRAM->data0) return 0;
    term_readline(buf,128); /*same as getline in c*/
    return 1;
}

static void init_window (void){
     finestra.bufpos=0;
     }

static int load_ram_block(char* addr){
    int i=0;
    tmp_t.tmp_len=0;
    int pos=-1;
    for (i=0; i<BLOCK_SIZE; i++, addr++)
         bbuffer[i]=*addr;
    for (i=BLOCK_SIZE-1; i>=0 && pos==-1; i--)
    {
    	if (bbuffer[i] == ' ') pos=i;
    }
    for (i=pos; i<BLOCK_SIZE; i++,tmp_t.tmp_len++)
    	tmp_t.tmp_c[tmp_t.tmp_len] = bbuffer[i];
    return pos;
}
int bp=0;

static int display_window(int posSpace){
        term_puts_till_space(bbuffer,posSpace);
        return finestra.bufpos;
}

static void term_readline(char *buf, unsigned int count)
{
    int c;

    while (--count && (c = term_getchar()) != '\n')
        *buf++ = c;

    *buf = '\0';
}

