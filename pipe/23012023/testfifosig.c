/* 2.

Scrivere il programma fifosig e un programma di prova che scriva nella pipe il proprio pid e il numero di SIGUSR1 e controlli di ricevere SIGUSR1

*/

#include <signal.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

void signal_handler(int signo, siginfo_t* info, void* context) {
	printf("Ricevuto segnale %d dal PID %d\n", signo, info->si_pid);
}

int main(int argc, char* argv[]) {
	int fd = open("/tmp/ff", O_WRONLY);

	pid_t pid = getpid();
	pid_t cpid;
	int num = 15;

	char msg[32];
	snprintf(msg, sizeof(msg), "%d %d\n", pid, num);
	write(fd, msg, strlen(msg));
	close(fd);
  
  	printf("Scrivo su pipe condivisa segnale %d a pid %d\n", num, pid);

	struct sigaction sa;
	sa.sa_sigaction = signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;

	if (sigaction(SIGUSR1, &sa, NULL) == -1) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
	
	while(1) {
		// attende segnale
		pause();
	}

	return 0;
}
