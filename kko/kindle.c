#include "umps/libumps.h"
#include "umps/arch.h"
#include "umps/types.h"
#include "strutils.h"
#include "terminal.h"
#include "tapeutils.h"

#define RAM_BEGIN          (char*)0x20007000
#define BLOCK_SIZE         4096
typedef unsigned int u32;

/*this struct is used to prevent the last word of the block to be cut. That word will be entirely displayed in the next block*/
static struct tmp{ 
char tmp_c[4098];
int tmp_len;
}tmp_t;

dtpreg_t* tape0_register =(dtpreg_t *) DEV_REG_ADDR(IL_TAPE, 0); 	/* tape0_register is a pointer to the address of memory where the first tape device is mapped.*/
static char* endOfRAM; 			/*End of the ram block that ccontains the file read from the tape device*/
static char bbuffer[BLOCK_SIZE];	/*bbuffer that will contain the string tu be displayed on terminal*/
static char* block_pos=RAM_BEGIN;	/*pointer to the begin of the block that will be displayed*/


static int load_ram_block(char *addr);
static void term_readline();
static int ld_block();



static int loadTapess (int nfile)
{
    int fcount=0;
    int stat=-1;
    char* fbegin=RAM_BEGIN;	/*RAM address of the beginning of the file*/
    char* address= RAM_BEGIN;	/*RAM address that will point to the next block*/

   /*Loading of the files in the RAM. This will ONLY load the first nfile files, so when a file gets chosen, the next ones are not loaded*/

    do{
        if (stat==EOF){		/*if a file is finished, update values of fcount and fbegin */ 
            fbegin=address;
            fcount++;
            }
        stat = loadblock(tape0_register,address);	/*loading the next block and saving the obtained tape marker in stat*/
	if ((stat==EOF || stat==EOT ) && fcount == nfile)
            break;
        address+=BLOCK_SIZE;	/*addressing the next block*/
    }while(stat != EOT  && stat!=-1);
    endOfRAM = address+BLOCK_SIZE;
    if (fcount == nfile)
        return (int) fbegin;
    else return 0;     /*in case there is no file having the index written on the terminal, return 0*/
}


void main(void)
{
    char a[128];
    int n=0;
    int pos;
    int ld_stat;

    /*Choose a file. The index of the files begins at 0*/

    term_puts("File to read: ");
    do{
    	term_readline(a, 128);
    	n=strtoi(a);
        if (n==-1)
             term_puts("\nInvalid value, retry.");
    }while (n==-1);

    if ((pos=loadTapess(n))==0){ term_puts("File does not exist in tape."); WAIT(); *((u32 *) MCTL_POWER) = 0x0FF;}
    else if (pos==-1) { term_puts("Tape error."); WAIT(); *((u32 *) MCTL_POWER) = 0x0FF;}
    else block_pos=(char*)pos;
    
    do ld_stat=ld_block(); while (ld_stat);
    /* Go to sleep and power off the machine if anything wakes us up */
    WAIT();
    *((u32 *) MCTL_POWER) = 0x0FF;
    while (1) ;
}

static int ld_block(void){
    int posSpace;
    char buf[128];

    /*load a block of 4096 bytes to the bbuffer, except the last word. posSpace gets the position of the last space (' '), in order to
      prevent the last word to be cut. That word will be entirely seen during the visualization of the next block*/

    posSpace = load_ram_block(block_pos);

    if (posSpace == -1) term_puts(bbuffer);     /*posSpace has a value of -1 when there is no space in the entire block. So visualize all the bbuffer*/
    else{
	 term_puts_till_space(bbuffer,posSpace);     /*visualize except last word*/
         if (block_pos+BLOCK_SIZE == endOfRAM){      /*if it's the last block, visualize the last word too and return 0*/
		term_puts(tmp_t.tmp_c, tmp_t.tmp_len);
		return 0;
         }
    }
    block_pos+=BLOCK_SIZE;

    term_readline(buf,128); /*same as getline in c. It waits for enter to be pressed*/

    if (posSpace != -1) term_puts_till_space(tmp_t.tmp_c, tmp_t.tmp_len);   /*visualize the last word*/

    if (block_pos<endOfRAM)
        return 1;
    else
        return 0;
}

static int load_ram_block(char* addr){
    int i=0;
    tmp_t.tmp_len=0;
    int pos=-1;

    for (i=0; i<BLOCK_SIZE && addr<endOfRAM; i++, addr++)    /*load the block from RAM to bbuffer*/
         bbuffer[i]=*addr;

    for (; i>=0 && pos==-1; i--)       /*search for the last space*/
    {
    	if (bbuffer[i] == ' ') pos=i;
    }

    for (i=pos+1; i<BLOCK_SIZE; i++,tmp_t.tmp_len++){      /*put the last word in tmp_t*/
    	tmp_t.tmp_c[tmp_t.tmp_len] = bbuffer[i];
    }
    return pos;
}


static void term_readline(char *buf, unsigned int count)
{
    int c;

    while (--count && (c = term_getchar()) != '\n')
        *buf++ = c;

    *buf = '\0';
}

