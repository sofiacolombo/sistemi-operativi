/* 1. 

Scrivere un programma fifotext che:
* crei una named pipe (FIFO) al pathname indicato come primo (e unico) argomento.
* apra la named pipe in lettura
* stampi ogni riga di testo ricevuta
* se la named pipe viene chiusa la riapra
* se riceve la riga "FINE" termini cancellando la named pipe.
Esempio:
fifotext /tmp/ff
....
se in un altra shell si fornisce il comando: "echo ciao > /tmp/ff", fifotext stampa ciao e rimane in attesa (questo esperimento si può provare più volte). Con il comando "echo FINE > /tmp/ff" fifotext termina.

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#define BUF 1024

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Manca il parametro pathname \n");
		exit(EXIT_FAILURE);
	}
	char* fifo = argv[1];

	if (mkfifo(fifo, 0755) < 0 && errno != EEXIST) {
		perror("mkfifo");
		exit(EXIT_FAILURE);
	}
	printf("creo la pipe %s \n", fifo);

	char* buf[BUF];
	int fd;
	ssize_t bytes;
	
	int fine = 0;
	while (1) {
		// ... in attesa 
		fd = open(fifo, O_RDONLY);
		bytes = read(fd, buf, sizeof(buf) -1);
		buf[bytes] = '\0';

		printf("%s", buf);
		if (!strncmp((char*)buf, "FINE", 4)) {
			printf("fine\n");
			break;
		}
	}
	unlink(fifo);
	printf("pipe eliminata\n");

	return 0;
}
