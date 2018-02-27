#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

void parentdone(int signum) {}

int
main()
{
  int pid;

  if ((pid = fork())== -1) {    /* create a child process */
    perror("fork() failed");
    exit(1);
  }

  if (pid == 0) {
    /* child process */
    printf("Hello, ");
    fflush(stdout);
  }
  else {
    /* parent process */
    wait(NULL);       /* wait the child exit */
    printf("World!\n");
    fflush(stdout);
  }

  exit(0);
}