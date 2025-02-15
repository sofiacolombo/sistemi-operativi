/* 1.

Scrivere la funzione:
char **vreaddir(const char *path)
che restituisca l'elenco dei nomi dei file in una directory come vettore di stringhe terminato con un puntatore NULL. (lo stesso formato di argv o envp).
Il vettore e le stringhe dei nomi sono allocate dinamicamente. 

Completare l'esercizio con un programma principale che testi il corretto funzionamento della funzione vreaddir.

*/

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#define LEN 25

char **vreaddir(const char* path, int num) {
	char** args = (char**)malloc(num * sizeof(char*));
	DIR* dir;
	struct dirent* entry;
	
	dir = opendir(path);
	if (dir < 0) exit(EXIT_FAILURE);
	
	int i = 0;
	while (entry = readdir(dir)) {
		if (entry->d_type == DT_REG) {
			args[i] = (char*)malloc( (strlen(entry->d_name)+1) *sizeof(char));
			if (!args[i]) {
				perror("malloc");
				exit(EXIT_FAILURE);
			}
			strcpy(args[i], entry->d_name);
			i++;
		}
	}
	closedir(dir);
	args[i] = NULL;

	return args;
}
// idea di usare pipe e cpid per fare "ls | wc -l" e ottenere un numero di linee non stimato
int linee(char* path) {
	pid_t cpid;
	int pipefd[2];

	pipe(pipefd);

	cpid = fork();
	if (cpid == 0) {
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		char* args[] = {"/bin/ls", path, NULL};

		execve(args[0], args, NULL);
		perror("child");
		exit(EXIT_FAILURE);
	} 
	close(pipefd[1]);
	char buf[1024];
	size_t bytes;
	int count = 0;

	bytes = read(pipefd[0], buf, sizeof(buf)-1);
	buf[bytes] = '\0';
	close(pipefd[0]);

	char* line;
	line = strtok(buf, "\n");
	while (line != NULL) {
		count++;
		line = strtok(NULL, "\n");
	}
	wait(NULL);

	return count;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Manca parametro pathname\n");
		exit(EXIT_FAILURE);
	}
	int num = linee(argv[1]);
	
	char** elenco = vreaddir(argv[1], num);
	
	// da stampare
	for (int i = 0; elenco[i] != NULL; i++) {
		printf("%s, ", elenco[i]);
	}
	printf("\n");
	for (int j = 0; elenco[j] != NULL; j++) {
		free(elenco[j]);
	}
	free(elenco);

	return 0;
}
