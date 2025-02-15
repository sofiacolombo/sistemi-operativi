/* 1.

Scrivere un secondo programma argrecv che preso in input l'output di argsend esegua il comando con gli argomenti passati a argsend. Per esempio:
$ ./argsend ls -l /tmp | ./argrecv
total 8988
-rw-r--r-- 1 renzo renzo 150532 Jan 9 16:57 ....
.....

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define LEN 128

int main(int argc, char* argv[]) {
	char buf[LEN];
	ssize_t bytes; 
	char* args[10];

	bytes = read(STDIN_FILENO, buf, sizeof(buf)-1);
	buf[bytes] = '\0';
  
  int i = 0;
  args[i] = buf; // prende ls 
  for (int j = 0; j < bytes; j++) {
    if (buf[j] == '\0' && i < 10 -1) {
      args[++i] = &buf[j+1];
    }
  }
  args[i] = NULL;
  
  execvp(args[0], args);

  perror("execve");
	
  return 0;
}
