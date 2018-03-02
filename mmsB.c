#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include<limits.h>
#include<sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>

typedef struct{
  int min;
  int max;
  int sum;
  int count;
} info_struct;

int *values, *values1, *values2;
int size, half;
FILE *out;
 
int main(int argc, char *argv[]) {
   clock_t begin = clock();

   FILE *input = fopen(argv[1], "r");
   out = fopen(argv[2], "w+");
   values = malloc(100000*sizeof(int));
 
   size = 0;
   while(fscanf(input, "%d\n", &values[size++]) != EOF);
   half = size/2;
   values1 = malloc(half*sizeof(int));
   for(int i = 0; i < half; i++){
      values1[i] = values[i];
     // printf("%d\n", values1[i]);
   }
   values2 = malloc(half*sizeof(int));
   for(int i = half; i < size - 1; i++){
      values2[abs(size - half - i)] = values[i];
     // printf("%d\n",values2[abs(size - half - i)]);
   }

   info_struct info;
   info.min = INT_MAX;
   info.max = INT_MIN;
   info.sum = 0;
  
   // Second pipe to send min,max,sum struct from child
 
   int fd2[2];  // Used to store two ends of second pipe
 
   pid_t p;
 
   if (pipe(fd2)==-1) {
      fprintf(stderr, "Pipe Failed" );
      return 1;
   }
 
   p = fork();
 
   if (p < 0) {
      fprintf(stderr, "fork Failed" );
      return 1;
   }
 
   // Parent process
   else if (p > 0) {
      printf("hi, i\'m process %d and my parent is %d\n", getpid(), getppid());
      fprintf(out,"hi, i\'m process %d and my parent is %d\n", getpid(), getppid());

 
      int temp;
      // find min, max, sum of array
      for(int i = 0; i < half; i++) {
         temp = values1[i];
         //printf("values1[%d] = %d\n", i, temp);
         if(temp < info.min){ info.min = temp; }
         if(temp > info.max){ info.max = temp; }
         info.sum += temp;
      }

      // Wait for child to send a string
      wait(NULL);
 
      close(fd2[1]); // Close writing end of second pipe
      
      info_struct info2;
      // Read info_struct from child, print it and close reading end.
      read(fd2[0], &info2, 3*sizeof(int));
      
      if (info2.min < info.min) { info.min = info2.min; }
      if (info2.max > info.max) { info.max = info2.max; }
      info.sum += info2.sum;

      clock_t end = clock();
      double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
      printf("max = %d min = %d sum = %d \ntime = %f \n", info.max, info.min, info.sum, time_spent);
      fprintf(out, "max = %d min = %d sum = %d \ntime = %f \n", info.max, info.min, info.sum, time_spent);

      close(fd2[0]);

   }
 
   // child process
   else {
      printf("hi, i\'m process %d and my parent is %d\n", getpid(), getppid());  
      fprintf(out, "hi, i\'m process %d and my parent is %d\n", getpid(), getppid());    
 
      info_struct val2_info;
      val2_info.min = INT_MAX;
      val2_info.max = INT_MIN;
      val2_info.sum = 0;

      int temp;
      // find min, max, sum of array
      for(int i = 0; i < half - 1; i++) {
         temp = values2[i];
         //printf("values2[%d] = %d\n", i, temp);
         if(temp < val2_info.min){ val2_info.min = temp; }
         if(temp > val2_info.max){ val2_info.max = temp; }
         val2_info.sum += temp;
      }

      //printf("min: %d, max: %d, sum: %d \n", val2_info.min, val2_info.max, val2_info.sum);

      // Close both reading ends
      close(fd2[0]);

      // Write concatenated string and close writing end
      write(fd2[1], &val2_info, 3*sizeof(int));
      close(fd2[1]);

      exit(0);
   }
}