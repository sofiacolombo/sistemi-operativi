/* 2.

Estendere i programmi dell'es.1 per consentire il trasferimento di stringhe di lunghezza arbitraria (iterando il procedimento 8 caratteri alla volta).

*/ 

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#define LEN 8

void signal_handler(int signo, siginfo_t *info, void *context) {
	// valore da gestire
	char str[LEN+1];
	uint64_t packed = (uint64_t)info->si_value.sival_ptr;

	for (int i = 0; i < 8; i++) {
        str[i] = (packed >> (i * 8)) & 0xFF; // Estrai ogni byte
    }
    str[LEN] = '\0';	
	
	printf("Ricevuto segnale %d dal PID %d \n", signo, info->si_pid);
	printf("Received value: %s\n", str);
}

int main(int argc, char* argv[]) {
	pid_t pid = getpid();
	printf("Pid di rx: %d \n", pid);
	
	struct sigaction sa; 
	sa.sa_flags = SA_SIGINFO | SA_RESTART;
	sa.sa_sigaction = signal_handler;
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGUSR1, &sa, NULL) == -1 ) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
  

	while (1) {
		// attendi segnali
		pause();
	}

	return 0;
}
