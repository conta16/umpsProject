	# Cross toolchain variables
# If these are not in your path, you can make them absolute.
XT_PRG_PREFIX = mipsel-linux-gnu-
CC = $(XT_PRG_PREFIX)gcc
LD = $(XT_PRG_PREFIX)ld
#umps2 installation directory, change if incorrect.
ifneq ($(wildcard /usr/bin/umps2),)
	UMPS2_DIRECTORY = /usr
else
	UMPS2_DIRECTORY = /usr/local
endif

CFLAGS_LANG = -ffreestanding -ansi
CFLAGS_MIPS = -mips1 -mabi=32 -mno-gpopt -std=gnu11 -G 0 -mno-abicalls -fno-pic -mfp32 -nostdlib
CFLAGS = $(CFLAGS_LANG) $(CFLAGS_MIPS) -I$(UMPS2_DIRECTORY)/include/umps2/ -I. -Wall -O0

all : kernel.core.umps

kernel.core.umps : kernel
	umps2-elf2umps -k kernel

kernel : asl.o pcb.o utils.o init.o syscall.o scheduler.o interrupt.o main.o handlers.o p2test_rikaya.o
	$(LD) -o kernel $^ $(UMPS2_DIRECTORY)/lib/umps2/crtso.o $(UMPS2_DIRECTORY)/lib/umps2/libumps.o -G 0 -nostdlib -T $(UMPS2_DIRECTORY)/share/umps2/umpscore.ldscript

clean :
	rm -f *.o kernel kernel.*.umps

%.o : %.c %.h
	$(CC)  $(CFLAGS) -o $@ -c $<
