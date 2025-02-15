/* 

(undo dell'esercizio 1) Scrivere un programma che sostituisca tutti i link simbolici presenti nella directory corrente che puntano a ... /nomefile con i veri file che l'esercizio 1 aveva spostato nella directory tre punti. Usare la system call rename per fare la sostituzione in modo atomico (in nessun istante il file deve risultare inesistente). 

*/


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#define MAX_PATH 1024

int main(int argc, char* argv[]) {
	char current_dir[MAX_PATH];

	if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
		perror("Errore nel prendere la directory"); 
		return -1;
	}

	DIR* dir;
	struct dirent* entry;
	
	dir = opendir("./...");
	if (dir == NULL) {
		perror("Errore apertura directory\n");
		return -1;
	}
	
	while ((entry = readdir(dir)) != NULL) {
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..") )  {
			continue;
		}

		if (entry->d_type == DT_REG) {
			// muovo ogni file alla directory corrente
			char oldpath[MAX_PATH], newpath[MAX_PATH];
			snprintf(oldpath, sizeof(oldpath), "%s/.../%s", current_dir, entry->d_name);
			snprintf(newpath, sizeof(newpath), "%s/%s", current_dir, entry->d_name);
			
			// unlink dei symlink
			if (unlink(entry->d_name) == 0) {
				printf("Link di %s rimosso\n", entry->d_name);
			} else {
				perror("Errore unlink dei symlink");
			}
			// muovo i file
			if (rename(oldpath, newpath) == 0) {
				printf("Mosso file %s in %s\n", entry->d_name, current_dir);
			} else {
				perror("Errore muovere file");
			}
			
		}
	}

	closedir(dir);
	return 0;
}
