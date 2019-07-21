------------ README ------------

- Contenuto di phase1.5.tar.gz:

	- header file inclusi:

		- asl.h 
		- const.h
		- init.h
		- interrupt.h
		- listx.h
		- pcb.h
		- scheduler.h
		- syscall.h
		- utils.h
		- types_rikaya.h

	- Sorgenti:

		- asl.c
		- init.c
		- interrupt.c
		- pcb.c
		- scheduler.c
		- syscall.c
		- utils.c
		- main.c


- Configurazione:

	PREREQUISITI: bisogna avere umps2 installato sul proprio device. Per maggiori informazioni: https://github.com/tjonjic/umps

	- sposta il file "phase1.5.tar.gz" nella directory desiderata
	- decomprimi il file "phase1.5.tar.gz" 
	- lancia il comando "make" per la generazione dei file opportuni al kernel su umps2
	- lancia il comando "make clean" se si vuole cancellare i file creati da "make"

	Una volta caricata la macchina su umps2, bisogna deselezionare il checkbox "Exception" per un'esecuzione corretta del codice

- Note:

	Questa è la fase preliminare alla creazione del kernel. Se lo si vuole vedere completato: https://github.com/conta16/umpsProject/tree/master/phase2

- Autori:

	Informazioni sugli autori le puoi trovare in AUTHORS.txt

- Licenza:

	Informazioni sulla licenza le puoi trovare in LICENSE.txt
