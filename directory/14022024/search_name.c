/*

Scrivere un programma search_name che deve cercare nel sottoalbero della directory corrente tutti i file eseguibili con un nome file specifico passato come primo e unico parametro indicando per ogni file il tipo di eseguibile (script o eseguibile binario).
Ad esempio il comando:
./search_name testprog
deve cercare i file eseguibili chiamati testprog nell'albero della directory corrente. Poniamo siano ./testprog, ./dir1/testprog, ./dir/dir3/testprog, search_name deve stampare:
. /testprog: script
./dir1/testprog: ELF executable
./dir/dir3/testprog: ELF executable

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>

#define BUF_LEN 1024

int is_elf(char *file) {
	int fd = open(file, O_RDONLY);
	if (!fd) { perror("open"); return -1;}
	
	unsigned char magic[4];
	ssize_t bytes;
	if ((bytes = read(fd, magic, 4)) == -1) {
		close(fd);
		return 0;
	}
	close(fd);

	return (magic[0] == 0x7F && magic[1] == 'E' && magic[2] == 'L' && magic[3] == 'F');
}

int is_script(char* file) {
	int fd = open(file, O_RDONLY);
	if (!fd) { perror("open"); return -1;}
	
	unsigned char magic[2];
	ssize_t bytes;
	if ((bytes = read(fd, magic, 2)) == -1) {
		close(fd);
		return 0;
	}
	close(fd);
	
	return (magic[0] == '#' && magic[1] == '!');
}

void checkDir(char* pathdir, char* name) {
	char pathname[BUF_LEN];
	DIR* dir;
	struct dirent* entry;
  
  	printf("Apro directory %s\n", pathdir);
  
	dir = opendir(pathdir);
	while ((entry = readdir(dir) ) != NULL) {
		if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..") ) {
      		continue;
    	}
		snprintf(pathname, sizeof(pathname), "%s/%s", pathdir, entry->d_name);

		if (entry->d_type == DT_DIR) {
			checkDir(pathname, name);
		}

		if ( !strcmp(entry->d_name, name) ) {
			if (is_elf(pathname)) {
				printf("%s: ELF executable \n", pathname);
			} else if (is_script(pathname)) {
				printf("%s: script\n", pathname);
			}
		}
	}

	closedir(dir);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
		fprintf(stderr, "Errore: manca il parametro name\n");
		exit(EXIT_FAILURE);
	}
	char curr_dir[BUF_LEN];
	
	if (getcwd(curr_dir, sizeof(curr_dir)) == NULL) {
		perror("Errore get cwd");
		exit(EXIT_FAILURE);
	} 
	printf("Directory corrente %s \n", curr_dir);
	checkDir(curr_dir, argv[1]);	

	return 0;
}
