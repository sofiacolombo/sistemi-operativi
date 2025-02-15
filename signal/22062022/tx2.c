/* 2.

Estendere i programmi dell'es.1 per consentire il trasferimento di stringhe di lunghezza arbitraria (iterando il procedimento 8 caratteri alla volta).

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
	
	pid_t rxpid = atoi(argv[1]);
	char* str = argv[2];	
	size_t len = strlen(str);

    for (size_t i = 0; i < len; i += LEN) {
        char chunk[LEN + 1];
        strncpy(chunk, str + i, LEN);
        chunk[LEN] = '\0';

		uint64_t packed = 0;
		for (int i = 0; i < 8 && chunk[i] != '\0'; i++) {
			packed |= ((uint64_t)chunk[i] << (i * 8)); 
		}
	
		union sigval sig_value;
		sig_value.sival_ptr = (void*)packed;

		if (sigqueue(rxpid, SIGUSR1, sig_value) == -1) {
			perror("sigqueue");
			exit(EXIT_FAILURE);
		}

   		printf("Inviato messaggio %s a %d \n", chunk, rxpid); 
    }
	
	
	return 0;
}
