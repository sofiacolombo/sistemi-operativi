/* 2.

Scrivere un programma pargrcv che crei una named pipe (il pathname è passato come parametro) e quando si ridireziona nella named pipe la sequenza di caratteri creata da argsend dell'esercizio 1, pargrcv deve eseguire il comando.
$ ./pargrcv /tmp/mypipe
crea la named pipe /tmp/mypipe e si mette in attesa.
Da un'altro terminale il comando:
$ ./argsend ls -l /tmp > /tmp/mypipe
fa eseguire il comando "ls -l /" a pargrcv

*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#define LEN 256

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Manca pathname\n");
		exit(EXIT_FAILURE);
	}
	char* fifo = argv[1];

	if ( mkfifo(fifo, 0755) < 0 && errno != EEXIST) {
		perror("mkfifo");
		exit(EXIT_FAILURE);
	}
	printf("pipe %s creata\n", fifo);
	pid_t cpid;
	ssize_t bytes; char buf[LEN];
	char* args[10]; 
	
	while(1) {
		int fd = open(fifo, O_RDONLY);
		
		bytes = read(fd, buf, LEN-1);
		buf[bytes] = '\0';
		close(fd);

		int i = 0;
		args[i] = buf;
		for (int j = 0; j < bytes; j++) {
    		if (buf[j] == '\0' && i < 10 -1) {
      			args[++i] = &buf[j+1];
   			}
  		}
  		args[i] = NULL;
			
		cpid = fork();
		if (cpid == 0) {
			execvp(args[0], args);
			
			perror("exec");
			exit(EXIT_FAILURE);
		}
		wait(NULL);
	}
	
	return 0;
}
