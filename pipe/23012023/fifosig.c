/* 2.

Il programma fifosig è una estensione di fifotext. Le righe che riceve attraverso la named pipe sono composte da due numeri, il pid di un processo e il numero di un segnale. Per ogni riga correttamente formata il segnale indicato viene mandato al processo indicato dal pid.
In un esempio simile la precedente il comando "echo 12345 15 > /tmp/ff" deve causare l'invio del segnale 15 al processo 12345.
Scrivere il programma fifosig e un programma di prova che scriva nella pipe il proprio pid e il numero di SIGUSR1 e controlli di ricevere SIGUSR1.

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
#include <signal.h>

#define BUF 1024

int manda_signal(pid_t pid, int signal) {
	union sigval sig_value;
	sig_value.sival_int = signal;

	if ( sigqueue(pid, SIGUSR1, sig_value) == -1) {
		perror("sigqueue");
		return -1;
	}
	printf("Inviato signal %d a %d\n", signal, pid);
	
	return 0;
}

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

	char buf[BUF];
	int fd;
	ssize_t bytes;
	pid_t pid; int signal = 0;
	
	int fine = 0;
	while (1) {
		// ... in attesa 
		fd = open(fifo, O_RDONLY);
		if (( bytes = read(fd, buf, sizeof(buf) -1) ) < 0) {
			close(fd);
			perror("bytes");
			exit(EXIT_FAILURE);
		}
		buf[bytes] = '\0';

		printf("Ricevuto %s", buf);
		if (!strncmp((char*)buf, "FINE", 4)) {
			printf("fine\n");
			break;
		}
		// 
		char copy[BUF];
		strncpy(copy, buf, BUF);
		copy[BUF-1] = '\0';

		char* line = strtok(copy, " ");
		pid = (pid_t) atoi(line);
		
		line = strtok(NULL, " ");
		signal = atoi(line);
		
		if (manda_signal(pid, signal) < 0) {
			close(fd);
			exit(EXIT_FAILURE);
		}
		close(fd);
	}

	unlink(fifo);
	printf("pipe eliminata\n");

	return 0;
}
