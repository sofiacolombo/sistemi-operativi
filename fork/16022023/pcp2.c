/* 2.

Usando “getopt” consentire di scegliere il grado di parallelismo voluto:
pcp -j 8 file1 file2
deve creare 8 processi, ogni processo copia 1/8 del file.

*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUF 1024

void copia(int fd, int fd2, off_t start, off_t end) {
	char buf[BUF];
	ssize_t bytes;
	ssize_t total = 0;

	lseek(fd, start, SEEK_SET);
	lseek(fd2, start, SEEK_SET);

	while (total < (end - start) ) {
		size_t to_read = ((end - start) - total < BUF) ? (end - start) - total : BUF;
		bytes = read(fd, buf, to_read);

		if (bytes <= 0) break;
	
		write(fd2, buf, bytes);
		total += bytes;
	}
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		fprintf(stderr, "./pcp <file1> <file2> -j <parallelismo>\n");
		exit(EXIT_FAILURE);
	}	
	int opt; 
	int jv = 1;
	while ((opt = getopt(argc, argv, "j:")) != -1) {
		switch (opt) {
			case 'j': 
				jv = atoi(optarg);
				printf("Grado di parallelismo %d\n", jv);
				break;
			case '?': 
				fprintf(stderr, "Uso %s [-j]\n");
				exit(EXIT_FAILURE);
				break;
		}
	}
	int fd, fd2;
	char buf[BUF];
	
	fd = open(argv[optind], O_RDONLY);	
	fd2 = open(argv[optind+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	
	off_t file_size1 = lseek(fd, 0, SEEK_END); // = dim file + offset 
	if (file_size1 < 0) {
		close(fd); close(fd2);
		perror("lseek");
		exit(EXIT_FAILURE);
	}
	off_t offset = file_size1 / jv; // offset in base a 1/job

	off_t file_size2 = lseek(fd, 0, SEEK_END);
	if (file_size2 < 0) {
		close(fd); close(fd2);
		perror("lseek");
		exit(EXIT_FAILURE);
	}
	
	int start = 0;
	for(int i = 0; i < jv; i++) {
		pid_t cpid = fork();
		if (cpid == 0) {
			copia(fd, fd2, start, (i+1) * offset);

			printf("Scritta %d meta'\n", i+1);
			exit(EXIT_SUCCESS);
		}
		printf("%d\n", start);
		start = i*offset;
	}
	wait(NULL);

	close(fd);
	close(fd2);

	return 0;
}
