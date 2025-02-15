/* 2.

Rielaborare l'esercizio precedente per fare in modo che il vettore e le stringhe dei nomi siano allocati con una unica operazione malloc (in modo da poter liberare lo spazio usato da vreaddir con una unica operazione free.)

*/

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

#define LEN 50 // numero max di file 
#define MAX_LEN 255 // numero massimo nome file 

char **vreaddir(const char* path) {
	DIR* dir;
	struct dirent* entry;
	
	dir = opendir(path);
	if (dir < 0) exit(EXIT_FAILURE);
	
	char** args = (char**)malloc(LEN * sizeof(char*) + LEN * sizeof(char) * MAX_LEN);
	int i = 0;
  
  char* string_block = (char*)(args + LEN);
  
	while (entry = readdir(dir)) {
		if (entry->d_type == DT_REG) {
      		args[i] = string_block + (i * MAX_LEN);

			if (!args[i]) {
				perror("malloc");
				exit(EXIT_FAILURE);
			}
			strncpy(args[i], entry->d_name, MAX_LEN-1);
      		args[i][MAX_LEN-1] = '\0';   
			i++;
		}
	}
	closedir(dir);
	args[i] = NULL;

	return args;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Manca parametro pathname\n");
		exit(EXIT_FAILURE);
	}

	char** elenco = vreaddir(argv[1]);
	
	// da stampare
	for (int i = 0; elenco[i] != NULL; i++) {
		printf("%s, ", elenco[i]);
	}
	printf("\n");
	free(elenco);

	return 0;
}
