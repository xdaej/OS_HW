#include "info.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

FILE *writeFile;
int shmid;
int *numbers;
int size;
int perProcess;

void process(int start, info_struct *info, int perProcessRemainder){
  if (start >= size) return;
  fprintf(writeFile, "hi, i\'m process %d and my parent is %d\n", getpid(), getppid());
  
  int end = start+perProcess;
  if (perProcessRemainder > 0) end++;

  int fd[2];
  pipe(fd);
  pid_t childpid;

  fflush(writeFile);
  childpid = fork();

  if (childpid == 0){
    close(fd[0]);
    process(end, info, perProcessRemainder-1);
    write(fd[1], info, sizeof(info_struct));
    exit(0);
  }
  else{
    close(fd[1]);

    while(start < end){
      int number = numbers[start];
      if (number < info->min) info->min = number;
      if (number > info->max) info->max = number;
      info->sum += number;
      info->count++;
      start++;
    }

    info_struct new_info;
    read(fd[0], &new_info, sizeof(info_struct));
    if (new_info.min < info->min) info->min = new_info.min;
    if (new_info.max > info->max) info->max = new_info.max;
    info->sum += new_info.sum;
    info->count += new_info.count;

    waitpid(childpid, NULL, WNOHANG);
  }
}

int main(int argc, char *argv[]){
  struct timeval start,end;
  gettimeofday(&start,NULL);

  shmid = shmget(IPC_PRIVATE, 1000000*sizeof(int), 0666 | IPC_CREAT);
  numbers = shmat(shmid, 0, 0);

  FILE *readFile = fopen("input_test.txt", "r");
  writeFile = fopen(argv[2], "w+");
  int numProcesses = atoi(argv[3]);

  size = 0;
  while(fscanf(readFile, "%d\n", &numbers[size++]) != EOF);
  perProcess = size/numProcesses;
  int perProcessRemainder = size%numProcesses;

  info_struct info = {.min = INT_MAX, .max = INT_MIN, .sum = 0, .count = 0};
  process(0, &info, perProcessRemainder);

  fprintf(writeFile, "min: %d\n", info.min);
  fprintf(writeFile, "max: %d\n", info.max);
  fprintf(writeFile, "sum: %d\n", info.sum);

  fclose(readFile);
  fclose(writeFile);

  shmdt(numbers);
  shmctl(shmid, IPC_RMID, NULL);

  gettimeofday(&end,NULL);
  printf("%ld microseconds\n", (end.tv_sec*100000 + end.tv_usec) - (start.tv_sec*100000 + start.tv_usec));

  return 0;
}