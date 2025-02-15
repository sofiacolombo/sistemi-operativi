/* 1.

Scrivere il programma stdin2pipe che prenda in input due righe di testo, ogni riga contiene un comando coi rispettivi parametri. Il programma deve lanciare entrambi i comandi in modo tale l'output del primo diventi input del secondo.
Per esempio, dato il file cmds:
ls -l
awk '{print $1}'
l'esecuzione di:
stdin2pipe < cmds
sia equivalente al comando:
ls -l | awk '{print $1}'

*/
// codice potrebbe non essere corretto

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_BUF 512
#define MAX_ARGV 10

void problemi(char* line, char *argv[], int* argc) { 
  char copy[MAX_BUF]; 
  strcpy(copy, line);

  char* word = strtok(copy, " ");
  while (word != NULL) {
    printf("Word: %s\n", word);
    argv[*argc] = strdup(word);
    if (argv[*argc] == NULL) {
      perror("strdup");
      exit(EXIT_FAILURE);
    }
    (*argc)++;
    word = strtok(NULL, " ");
  }
  argv[*argc] = NULL;

}

int main(int argc, char* argv[]) {
	char buf[MAX_BUF];
	char* argv1[MAX_ARGV], *argv2[MAX_ARGV];
	int linesRead = 0;
	char* line, *word;
	
	ssize_t bytesRead = read(STDIN_FILENO, buf, sizeof(buf) -1 );
	if (bytesRead < 0) {
		perror("read");
		return -1;
	}
	buf[bytesRead] = '\0';
  	char copy[MAX_BUF];
  	strcpy(copy, buf);
  	printf("%s \n", buf);

  	int argc1 = 0, argc2 = 0;
	line = strtok(copy, "\n"); // divido per linea 
  	while (line != NULL && linesRead < 2) {
    printf("Riga: %s, %d\n", line, linesRead);

    if (linesRead == 0) {
      problemi(line, argv1, &argc1);
      line = strtok(buf, "\n");
    }
    else if (linesRead == 1) 
      porcoddio(line, argv2, &argc2);

    linesRead++;
    line = strtok(NULL, "\n");
  }

  if (linesRead < 2) {
    fprintf(stderr, "Sono necessarie due righe di comando\n");
    return -1;
  }
   
	int pipefd[2];
	pid_t cpid, cpid2;

	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	cpid = fork();
	if (cpid == 0) {
		// figlio
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		
		execvp(argv1[0], argv1);
		
		perror("execve1");	
		exit(EXIT_FAILURE);
	}

	cpid2 = fork();
	if (cpid2 == 0) {
		close(pipefd[1]);
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]);
		
		execvp(argv2[0], argv2);
		perror("execve");
        exit(EXIT_FAILURE);
	}
	close(pipefd[0]);
	close(pipefd[1]);
	
	// padre
	waitpid(cpid, NULL, 0);
	waitpid(cpid2, NULL, 0);

  for (int i = 0; argv1[i] != NULL; i++) {
        free(argv1[i]);
    }
    for (int i = 0; argv2[i] != NULL; i++) {
        free(argv2[i]);
    }

	return 0;
}
