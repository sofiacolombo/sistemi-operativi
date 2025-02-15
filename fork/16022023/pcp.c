/* 1.

Fare un programma di copia parallelo di file.
pcp file1 file2
pcp deve creare due processi figli; mentre un processo copia la prima meta’ del file, l'altro deve copiare l’altra meta’.

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
		bytes = read(fd, buf, sizeof(buf)-1);

		if (bytes <= 0) break;
	
		write(fd2, buf, bytes);
		total += bytes;
	}

}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		fprintf(stderr, "./pcp <file1> <file2>\n");
		exit(EXIT_FAILURE);
	}
	
	int fd, fd2;
	pid_t cpid,cpid2;
	char buf[BUF];
	
	fd = open(argv[1], O_RDONLY);
	fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC);
	
	off_t file_size1 = lseek(fd, 0, SEEK_END); // = dim file + offset 
	if (file_size1 < 0) {
		close(fd); close(fd2);
		perror("lseek");
		exit(EXIT_FAILURE);
	}
	off_t offset = file_size1 / 2;

	off_t file_size2 = lseek(fd, 0, SEEK_END);
	if (file_size2 < 0) {
		close(fd); close(fd2);
		perror("lseek");
		exit(EXIT_FAILURE);
	}
	
	cpid = fork();
	if (cpid == 0) {
		copia(fd, fd2, 0, offset);

		printf("Scritta prima meta'\n");
		exit(EXIT_SUCCESS);
	}
	wait(NULL);
	
	cpid2 = fork();
	if (cpid2 == 0) {
		copia(fd, fd2, offset, file_size1);

		printf("Scritta seconda meta'\n");
		exit(EXIT_SUCCESS);
	}
	wait(NULL);
		
	close(fd);
	close(fd2);

	return 0;
}
