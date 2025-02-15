/*

Estendere l'esercizio 1 creando il programma inotimrun che consenta per ogni file in D l'esecuzione sequenziale di più programmi: I file da inserire in D possono contenere più comandi separati da una riga vuota.
Es:
/bin/ls
ls
/rmp

/bin/cat
cat
/etc/hostname

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/inotify.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PATH_MAX 1024
#define BUF_LEN 4096

int read_execfile(char* file, char* pathname) {
	int fd;
	char path[PATH_MAX];
	// apro il file
	snprintf(path, sizeof(path), "%s/%s", pathname, file);
	fd = open(path, O_RDONLY);
	if (fd < 0) {
		perror("Errore fd file");
		exit(EXIT_FAILURE);
	} else printf("Apro file %s\n", file);
	// leggo il file 
	char buf[BUF_LEN]; 
	ssize_t bytes = read(fd, buf, sizeof(buf) -1 );
	close(fd);

	if (bytes < 0) {
		perror("Errore lettura");
		close(fd);
		exit(EXIT_FAILURE);
	}

	buf[bytes] = '\0';

	char *line = strtok(buf, "\n");
	char* argv[10];
	int argc = 0;

	while(line != NULL && argc < 9) {
		argv[argc++] = line;
		line = strtok(NULL, "\n");
	}
	argv[argc] = NULL;
	printf("Righe trovate nel file:\n");
    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = \"%s\"\n", i, argv[i]);
    }
	// creo fork per eseguire il file 
	pid_t cpid, w; int status;
	cpid = fork();
	if (cpid < 0) {
		perror("Fork");
		exit(EXIT_FAILURE);
	} 
	char *envp[] = {NULL};
	if (cpid == 0) {
		if (argc > 0) {
			printf("Eseguo: %s\n", argv[0]);	
			execve(argv[0], &argv[1], envp);
			
      perror("errore execve");
      exit(EXIT_FAILURE);
		}
	} else {
		w = wait(&status);
		if (w == -1) { 
			perror("waitpid"); 
			exit(EXIT_FAILURE); 
		}
	}
	
	// cancello il file
	if (unlink(path) < 0) {
		perror("errore delete file");
		exit(EXIT_FAILURE);
	}
	printf("File cancellato\n");
	return 0;
}

int main(int argc, char* argv[]) {
	char curr_dir[PATH_MAX];

	if (getcwd(curr_dir, sizeof(curr_dir)) == NULL) {
		perror("Errore get cwd");
		exit(EXIT_FAILURE);
	}
	
	const struct inotify_event *event;	
	int fd, wd;
	char buf[BUF_LEN];
	
	fd = inotify_init();
	if (fd < 0) {
		perror("Errore inotify init");
		exit(EXIT_FAILURE);
	}
	char pathname[PATH_MAX];
	snprintf(pathname, sizeof(pathname), "%s/D", curr_dir);
   		
	wd = inotify_add_watch(fd, pathname, IN_CREATE | IN_MOVED_TO);
	if (wd < 0) {
		perror("errore watch");
		exit(EXIT_FAILURE);
	}
	printf("Directory in osservazione %s \n", pathname);
	
	for (;;) {
		int length = read(fd, buf, sizeof(buf));
		if (length < 0) {
			perror("error read");
			exit(EXIT_FAILURE);
		}
		if (length == 0) {
			printf("file read");
		}
		int i = 0;
		while(i < length) {
			struct inotify_event* event = (struct inotify_event *)&buf[i]; 
			if (event->len) {
				if (event->mask & IN_CREATE) {
					printf("File creato %s\n", event->name);
				} else if (event->mask & IN_MOVED_TO) {
					printf("File mosso nella directory %s\n", event->name);
				}
			}
			if (read_execfile(event->name, pathname) < 0) {
				perror("errore lettura/esecuzione file");
				continue;
			}

			i += sizeof(struct inotify_event) + event->len;
		}
	}	

	inotify_rm_watch(fd, wd);
	close(fd);
	return 0;
}
