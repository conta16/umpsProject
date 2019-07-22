# Cross toolchain variables
# If these are not in your path, you can make them absolute.
XT_PRG_PREFIX = mipsel-linux-gnu-
CC = $(XT_PRG_PREFIX)gcc
LD = $(XT_PRG_PREFIX)ld
#umps2 installation directory, change if incorrect.
UMPS2_DIRECTORY = /usr/local
CFLAGS = -ansi -mips1 -mfp32 -std=gnu11 -I$(UMPS2_DIRECTORY)/include/umps2 -I. -Wall

all : kernel.core.umps

kernel.core.umps : kernel
	umps2-elf2umps -k kernel

kernel : asl.o pcb.o utils.o p1.5test_rikaya_v0.o init.o syscall.o scheduler.o interrupt.o main.o
	$(LD) -o kernel $^ $(UMPS2_DIRECTORY)/lib/umps2/crtso.o $(UMPS2_DIRECTORY)/lib/umps2/libumps.o -nostdlib -T $(UMPS2_DIRECTORY)/share/umps2/umpscore.ldscript

clean :
	rm -f *.o kernel kernel.*.umps

%.o : %.c %.h
	$(CC)  $(CFLAGS) -o $@ -c $<
