/* 1.

Scrivere due programmi C, tx e rx: tx deve trasferire a rx stringhe di max 8 caratteri usando i valori assegnati ai segnali (il parametro value della funzione sigqueue).
Il programma rx deve per prima cosa stampare il proprio pid e attendere segnali.
Il programma tx ha due parametri, il pid did rx e il messaggio.
Es: tx 22255 sigmsg
(posto che rx sia in esecuzione con pid 22255, rx deve stampare sigmsg).

*/

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdint.h>

#define LEN 8

int main(int argc, char* argv[]) {
	if (argc < 3) {
		fprintf(stderr, "argomenti mancanti, pid e msg\n");
		exit(EXIT_FAILURE);
	} // argv[1] pid di rx, argv[2] msg da mandare
	
	printf("la stringa verra' sempre limitata a 8 caratteri \n");

	pid_t rxpid = atoi(argv[1]);
	char* str = argv[2];

	uint64_t packed = 0;
	for (int i = 0; i < 8 && str[i] != '\0'; i++) {
		packed |= ((uint64_t)str[i] << (i * 8)); 
	}
	
	union sigval sig_value;
	sig_value.sival_ptr = (void*)packed;

	if (sigqueue(rxpid, SIGUSR1, sig_value) == -1) {
		perror("sigqueue");
		exit(EXIT_FAILURE);
	}
	
	printf("Inviato messaggio %s a %d \n", str, rxpid);

	return 0;
}
