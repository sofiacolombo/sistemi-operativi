/*

Scrivere un programma che crei nella directory corrente (se non esiste già) una sottodirectory di nome ... (tre punti).
Tutti i file (regolari) presenti nella directory devono essere spostati nella sottodirectory ... (tre punti) e ogni file deve essere sostituito nella dir corrente con un link simbolico (relativo, non assoluto) alla nuova locazione. Usare la system call rename per fare la sostituzione in modo atomico (in nessun istante il file deve risultare inesistente).

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define MAX_PATH 1024

int main(int argc, char* argv[]) {
	// get current directory
	char current_dir[MAX_PATH];

	if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
			perror("Errore nell'ottenere la directory corrente");
			return -1;
	} else {
		printf("Current dir: %s \n", current_dir);
	}
	// check if directory ... Already exists
	char checkDir[MAX_PATH];
	snprintf(checkDir, sizeof(checkDir), "%s/...", current_dir);
	
	if (mkdir(checkDir, 0755) == -1) {
		if (errno == EEXIST) {
			printf("La directory %s gia' esiste \n", checkDir);
		} else {
			perror("errore creazione directory");
		}
	} else {
		printf("directory %s creata\n", checkDir);
	}
	// apro la directory
	DIR* dir;
	struct dirent* entry;
	
	dir = opendir(current_dir);

   	if (dir == NULL) {
		perror("errore apertura directory");
		return -1;
	}
	printf("apro directory %s \n", current_dir);
	
	while((entry = readdir(dir)) != NULL) {
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "...") ) {
			continue;
		}

		if (entry->d_type == DT_REG) {
			char target[MAX_PATH], linkpath[MAX_PATH];
			snprintf(target, sizeof(target), "%s/.../%s", current_dir, entry->d_name);
			snprintf(linkpath, sizeof(linkpath), "./%s", entry->d_name);
			
			// muovo il file
			char newPath[MAX_PATH];
			snprintf(newPath, sizeof(newPath), "%s/%s", current_dir, entry->d_name);
			if (rename(newPath, target) == 0) {
				printf("File %s mosso\n", entry->d_name);
			} else {
				perror("Errore muovere file \n");
			}
			
			// creo link
			if (symlink(target, linkpath) == 0) {
				printf("Creato link per %s \n", entry->d_name);
			} else {
				perror("Errore creazione symlink");
			}
		
		}
	}

	closedir(dir);
	return 0;
}
