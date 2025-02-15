/*

Il programma run_name deve cercare nel sottoalbero della directory corrente tutti i file eseguibili con un nome file specifico (primo parametro di run_name) e li deve mettere in esecuzione uno dopo l'altro passando i successivi parametri.
Ad esempio il comando:
./run_name testprog a b c
deve cercare i file eseguibili chiamati testprog nell'albero della directory corrente. Poniamo siano ./testprog, ./dir1/testprog, ./dir/dir3/testprog, run_name deve eseguire testprog a b c per 3 volte. Nella prima esecuzione la working directory deve essere la dir corrente '.', la seconda deve avere come working directory './dir1' e la terza './dir2/dir3'.

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

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

int eseguo(char* argv[]) {
	pid_t cpid;
	printf("eseguo %s \n", argv[1]);

	cpid = fork();
	if (cpid < 0) {perror("child"); return -1;}

	if (cpid == 0) {
		execve(argv[1], &argv[1], NULL);

		perror("execve");
		return -1;
	}
	wait(NULL);

	return 0;
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
			checkDir(pathname, name[1]);
		}

		if ( !strcmp(entry->d_name, name[1]) ) {
			if (is_elf(pathname)) {
				printf("%s: ELF executable \n", pathname);
			} else if (is_script(pathname)) {
				printf("%s: script\n", pathname);
			}

      if ( eseguo(name) < 0 ) {
          perror("eseguo");
          exit(EXIT_FAILURE);
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
	checkDir(curr_dir, argv);	

	return 0;
}
