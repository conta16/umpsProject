------------ README ------------

- Contenuto di phase2.tar.gz:

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
		- handlers.h

	- Sorgenti:

		- asl.c
		- init.c
		- interrupt.c
		- pcb.c
		- scheduler.c
		- syscall.c
		- utils.c
		- main.c
		- handlers.c


- Configurazione:

	PREREQUISITI: bisogna avere umps2 installato sul proprio device. Per maggiori informazioni: https://github.com/tjonjic/umps

	- sposta il file "phase2.tar.gz" nella directory desiderata
	- decomprimi il file "phase2.tar.gz"
	- lancia il comando "make" per la generazione dei file opportuni al kernel su umps2
	- lancia il comando "make clean" se si vuole cancellare i file creati da "make"

	Una volta caricata la macchina su umps2, bisogna deselezionare il checkbox "Exception" per un'esecuzione corretta del codice


- Autori:

	Informazioni sugli autori si possono trovare in AUTHORS.txt

- Licenza:

	Informazioni sulla licenza si possono trovare in LICENSE.txt
