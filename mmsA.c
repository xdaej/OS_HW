#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int *values;
int size;

int main(int argc, char *argv[]) {
  clock_t begin = clock();

  FILE *input = fopen(argv[1], "r");
  values = malloc(100000*sizeof(int));
 
  size = 0;
  while(fscanf(input, "%d\n", &values[size++]) != EOF);

  int min = INT_MAX;
  int max = INT_MIN;
  int sum = 0;
  int temp;

  //printf("size: %d\n", size);
  for(int i = 0; i < size - 1; i++){
    temp = values[i];
    if(temp > max) { max = temp; }
    if(temp < min && temp ) { min = temp; }
    //printf("%d\n", temp);
    sum += temp;
  }

  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("hi, i\'m process %d and my parent is %d\n", getpid(), getppid());
  printf("max = %d min = %d sum = %d \ntime = %f \n", max, min, sum, time_spent);

  FILE *out = fopen(argv[2], "w+");
  fprintf(out, "hi, i\'m process %d and my parent is %d\n", getpid(), getppid());
  fprintf(out, "max = %d \nmin = %d \nsum = %d \ntime = %f \n", max, min, sum, time_spent);
  
  return 0;
}