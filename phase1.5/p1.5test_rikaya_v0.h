#ifndef P1_5_TEST_RIKAYA_V0_H
#define P1_5_TEST_RIKAYA_V0_H

#include "const.h"
#include "listx.h"
#include <umps/libumps.h>
#include <umps/arch.h>

#include "pcb.h"

#define TRANSMITTED 5
#define TRANSTATUS 2
#define ACK 1
#define PRINTCHR 2
#define CHAROFFSET 8
#define STATUSMASK 0xFF
#define TERM0ADDR 0x10000250
#define DEVREGSIZE 16
#define READY 1
#define DEVREGLEN 4
#define TRANCOMMAND 3
#define BUSY 3

#define TOD_LO *((unsigned int *)0x1000001C)
#define TIME_SCALE *((unsigned int *)0x10000024)
#define RAMBASE *((unsigned int *)0x10000000)
#define RAMSIZE *((unsigned int *)0x10000004)
#define RAMTOP (RAMBASE + RAMSIZE)

#define SYS3 3
#define STEPS 6
#define GANTT_SIZE 20
#define TEST_PROCS 3

typedef unsigned int devreg;

static unsigned int get_microseconds();
static void delay_ms(unsigned int ms);

/******************************************************************************
 * I/O Routines to write on a terminal
 ******************************************************************************/

/* This function returns the terminal transmitter status value given its address */
devreg termstat(memaddr *stataddr);

/* This function prints a string on specified terminal and returns TRUE if 
 * print was successful, FALSE if not   */
unsigned int termprint(char *str, unsigned int term);

/* This function places the specified character string in okbuf and
 *	causes the string to be written out to terminal0 */
void addokbuf(char *strp);
/* This function places the specified character string in errbuf and
 *	causes the string to be written out to terminal0.  After this is done
 *	the system shuts down with a panic message */
void adderrbuf(char *strp);
void log_process_order(int process);

void test1();

void test2();

void test3();

#endif
