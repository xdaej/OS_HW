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

int *values, *values1, *values2, *values1a, *values1b, *values2a, *values2b;
int size, half, quarter;
FILE *out;
 

int * gchild_function(int i) {
   if(i == 0){ return values1a; }
   else if(i == 1) { return values1b; }
   else if(i == 2) { return values2a; }
   else { return values2b; }
}

int main(int argc, char *argv[]) {
   clock_t begin = clock();

   FILE *input = fopen(argv[1], "r");
   out = fopen(argv[2], "w+");
   values = malloc(100000*sizeof(int));
 
   size = 0;
   while(fscanf(input, "%d\n", &values[size++]) != EOF);
   half = size/2;
   quarter = half/2;
  
   //split array into halves
   values1 = malloc(half*sizeof(int));
   memcpy(values1, values, half*sizeof(int));
   values2 = malloc(half*sizeof(int));
   memcpy(values2, values + half, half*sizeof(int));

   //split halves into havles
   values1a = malloc(quarter*sizeof(int));
   memcpy(values1a, values1, quarter*sizeof(int));
   values1b = malloc(quarter*sizeof(int));
   memcpy(values1b, values1b + quarter, quarter*sizeof(int));
   values2a = malloc(quarter*sizeof(int));
   memcpy(values2a, values2, quarter*sizeof(int));
   values2b = malloc(quarter*sizeof(int));
   memcpy(values2b, values2 + quarter, quarter*sizeof(int));
   
   /*
   For checking if arrays were split properly
   for(int i = 0; i < quarter; i++) { printf("%d\n", values1a[i]); }
   for(int i = 0; i < quarter; i++) { printf("%d\n", values1a[i]); }
   for(int i = 0; i < quarter; i++) { printf("%d\n", values2a[i]); }
   for(int i = 0; i < quarter; i++) { printf("%d\n", values2b[i]); }
   */

   info_struct info;
   info.min = INT_MAX;
   info.max = INT_MIN;
   info.sum = 0;
  
   //pipe to send min,max,sum struct from child
   // Used to store two ends of second pipe
   int fd1[2];
   int fd2[2];  
   pid_t grandChild;
 
   if(pipe(fd1) == -1) {
      fprintf(stderr, "Pipe1 Failed" );
      return 1;
   }
   if(pipe(fd2) == -1) {
      fprintf(stderr, "Pipe2 Failed" );
      return 1;
   }

   pid_t pid = getpid();
   //Each child will have a different index.
   int index;
   for (int n = 0; n < 1; n++){
      if (pid > 0){
         pid = fork();
         index = n;  
      }
      else if (pid < 0){
         fprintf(stderr, "Fork error!");
      }
   }
   //child process
   if (pid == 0){
      printf("hi, i\'m process %d and my parent is %d\n", getpid(), getppid());  
      fprintf(out, "hi, i\'m process %d and my parent is %d\n", getpid(), getppid());    
 
      info_struct val1_info;
      val1_info.min = INT_MAX;
      val1_info.max = INT_MIN;
      val1_info.sum = 0; 
      int *tempar = gchild_function(index);
      int temp;
      // find min, max, sum of array
      for(int i = 0; i < quarter; i++) {
         temp = tempar[i];
         //printf("values1[%d] = %d\n", i, temp);
         if(temp < val1_info.min){ val1_info.min = temp; }
         if(temp > val1_info.max){ val1_info.max = temp; }
         val1_info.sum += temp;
      }
      //call grandchild
      grandChild = fork();

      close(fd2[1]); // Close writing end of second pipe
      
      info_struct info2;
      // Read info_struct from child, print it and close reading end.
      read(fd2[0], &info2, 3*sizeof(int));
      
      if (info2.min < val1_info.min) { val1_info.min = info2.min; }
      if (info2.max > val1_info.max) { val1_info.max = info2.max; }
      val1_info.sum += info2.sum;

      // Write concatenated string and close writing end
      write(fd1[1], &val1_info, 3*sizeof(int));
      close(fd1[1]);

      if (grandChild == -1) {
            perror("grandchild:");
            exit(0);
      }

      if (grandChild == 0) {
         // grandchild 
         printf("hi, i\'m process %d and my parent is %d\n", getpid(), getppid());  
         fprintf(out, "hi, i\'m process %d and my parent is %d\n", getpid(), getppid());    
    
         info_struct val2_info;
         val2_info.min = INT_MAX;
         val2_info.max = INT_MIN;
         val2_info.sum = 0;    

         int *tempar = gchild_function(index+2);
         int temp;
         // find min, max, sum of array
         for(int i = 0; i < quarter - 1; i++) {
            temp = tempar[i];
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

      exit(0);
   } else {
      //parent process

      printf("hi, i\'m process %d and my parent is %d\n", getpid(), getppid());  
      fprintf(out, "hi, i\'m process %d and my parent is %d\n", getpid(), getppid()); 

      close(fd1[1]);

      info_struct infoTemp;
      read(fd1[0], &infoTemp, 3*sizeof(int));
      if (infoTemp.min < info.min) { info.min = infoTemp.min; }
      if (infoTemp.max > info.max) { info.max = infoTemp.max; }
      info.sum += infoTemp.sum;

      close(fd1[0]);
      clock_t end = clock();
      double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
      printf("max = %d min = %d sum = %d \ntime = %f \n", info.max, info.min, info.sum, time_spent);
      fprintf(out, "max = %d min = %d sum = %d \ntime = %f \n", info.max, info.min, info.sum, time_spent);


   }
 
   return 0;
}