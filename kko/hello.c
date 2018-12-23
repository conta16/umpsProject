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
dtpreg_t *endOfTAPE;
static int load_ram_block();
static int display_window(int posSpace);
static void init_window();
static char bbuffer[BLOCK_SIZE];
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
char vet;

static struct pres_forw{
	unsigned int pres;
	unsigned int forw;
} k;

unsigned int stat;

static void loadTapess (int indexTape)
{
    /*I tell where the tape pointers will point*/
    int count=0;
    tape0_register = (dtpreg_t *) DEV_REG_ADDR(IL_TAPE, 0);
    tape0_register->data0 = RAM_BEGIN;
    k.pres = RAM_BEGIN;

   /*Loading of the files in the RAM. I use tmp_tape_register to iterate in the RAM and, at the end of each do while, I define where tape[i]_register
     must point (beginning of his file) and I make tmp_tape_register point to the next tape*/

    /*if (tape0_register->data0 == RAM_BEGIN) term_puts("diocane");
    if (tmp->data0 == RAM_BEGIN) term_puts("diocane2");*/
    while (tape0_register->data1 != EOT && tape0_register->status!=0){
        loadblock(tape0_register,tape0_register->data0);
        if (tape0_register->data1 == EOF || tape0_register->data1 == EOT){
		 count++;
		if (indexTape == count+1) k.pres = tape0_register->data0+BLOCK_SIZE;
		if (indexTape == count) k.forw = tape0_register->data0+BLOCK_SIZE;
	}
	tape0_register->data0 = tape0_register->data0+BLOCK_SIZE;
    }

    /*if (stat == tape0_register->data0) term_puts("diocane3");*/
    stat = tape0_register->data0;
    tape0_register->data0 = RAM_BEGIN;
    /*for (pointer = tape0_register->data0; tape0_register->data1 != EOT; pointer++)
    	if (*pointer == '\0') count++;*/
}


void main(void)
{
    char a[128];
    int i=1;
    unsigned int num_eof;
    indexTape = chooseTape()-48; /*which tape will be seen*/
    loadTapess(indexTape);
    tape0_register->data0 = k.pres;
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

    posSpace = load_ram_block(tape0_register->data0);
    display_window(posSpace); /*display block*/

    if (tape0_register->data0 == stat || tape0_register->data0 == k.forw) return 0;
    /*In this part, I make the pointer point to the next block, and if it's equal to the pointer of the next tape, then it returns 0*/
    /*if (tape0_register->data1 == EOF || tape0_register->data1 == EOT) return 0;*/
    term_readline(buf,128); /*same as getline in c*/
    return 1;
}

static void init_window (void){
     finestra.bufpos=0;
     }

static int load_ram_block(char *addr){
    int i=0;
    tmp_t.tmp_len=0;
    int pos=-1;
    for (i=0; i<BLOCK_SIZE && tape0_register->data0 != stat; i++, addr++, tape0_register->data0++)
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

