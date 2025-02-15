#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX_BUF 4096
#define MAX 128

int main(int argc, char* argv[]) {
	int pipefd[2];

	if (argc < 2) {
		perror("Manca parametro pathname");
		exit(EXIT_FAILURE);
	}
	printf("%s \n", argv[1]);

	char curr_dir[MAX];
	if (getcwd(curr_dir, sizeof(curr_dir)) == NULL) {
		perror("errore getcwd");
		exit(EXIT_FAILURE);
	}
  
  if (pipe(pipefd) < 0) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

	pid_t cpid;
	if ((cpid = fork()) < 0) {
		perror("fork");
		exit(EXIT_FAILURE);
	}
	
	if (cpid == 0) {
		// fai ls -l di argv[1]
		char *argv1[] = {"/bin/ls", "-l", argv[1], NULL};
		
		close(pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) < 0) {
      perror("dup");
      exit(EXIT_FAILURE);
    }
		close(pipefd[1]);
    printf("Child esegue %s\n", argv1[0]);
		execve(argv1[0], argv1, NULL);

		perror("child1");
		exit(EXIT_FAILURE);
	} else {
	
		char buf[MAX_BUF]; ssize_t bytes; 
    size_t total_bytes = 0;
		close(pipefd[1]);

		while ((bytes = read(pipefd[0], buf + total_bytes, sizeof(buf) - total_bytes - 1)) > 0) {
		    total_bytes += bytes;
		}
		buf[total_bytes] = '\0';

    printf("buffer %s\n", buf);
		char* line, copy[MAX_BUF];
		strcpy(copy, buf);
    	
		char perm[MAX], filename[MAX];
		char pathname[MAX], target[MAX];

		line = strtok(buf, "\n");
		printf("line %s\n", line);
		while (line != NULL) {
			if (sscanf(line, "%s %*d %*s %*s %*s %*s %*s %*s %s", perm, filename) == 2) {
				printf("Perm %s, filename %s\n", perm, filename);
				snprintf(pathname, sizeof(pathname), "%s/%s", curr_dir, perm);
				
				if (mkdir(pathname, 0755) < 0 && errno != EEXIST) {
          			perror("mkdir");
          			exit(EXIT_FAILURE);
        		}

				snprintf(pathname, sizeof(pathname), "%s/%s/%s", curr_dir, perm, filename);
				snprintf(target, sizeof(target), "%s%s", argv[1], filename);
				
				if (symlink(target, pathname) < 0) {
					perror("symlink");
					exit(EXIT_FAILURE);
				} 
				printf("Creo link da %s a %s \n", target, pathname);
			}
			line = strtok(NULL, "\n");
		}

		wait(NULL);
	}

	return 0;
}
