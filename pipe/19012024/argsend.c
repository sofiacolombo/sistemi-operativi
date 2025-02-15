/* 1.

Scrivere un programma argsend che converta i parametri del programma (da argv[1] in poi) in una unica sequenza di caratteri: vengono concatenati i parametri (compreso il terminatore della stringa).
Esempio di funzionamento:
$ ./argsend ls -l /tmp | od -c
0000000 l s \0 - l \0 / t m p \0
0000013

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define LEN 256

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "mancanza parametri\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 1; i < argc; i++) {
		for (int j = 0; j < strlen(argv[i]); j++) {
			printf("%c", argv[i][j]);
		}
		putchar('\0');
	}

	return 0;
}
