#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* ReadFile(char *filename) {
   char *buffer = NULL;
   int string_size, read_size;
   FILE *handler = fopen(filename, "r");

   if (handler) {
       // Seek the last byte of the file
       fseek(handler, 0, SEEK_END);
       // Offset from the first to the last byte, or in other words, filesize
       string_size = ftell(handler);
       // go back to the start of the file
       rewind(handler);

       // Allocate a string that can hold it all
       buffer = (char*) malloc(sizeof(char) * (string_size + 1) );

       // Read it all in one operation
       read_size = fread(buffer, sizeof(char), string_size, handler);

       // fread doesn't set it so put a \0 in the last position
       // and buffer is now officially a string
       buffer[string_size] = '\0';

       if (string_size != read_size) {
           // Something went wrong, throw away the memory and set
           // the buffer to NULL
           free(buffer);
           buffer = NULL;
       }

       // Always remember to close the file.
       fclose(handler);
    }

    return buffer;
}

int main() {
    int min, max, sum, temp;
    char *string = ReadFile("input_test.txt");
    /* if (string) {
        puts(string);
        free(string);
    }
    */
    min = string[0];
    max = string[0];
    sum = string[0];

    for(int i = 1; i < strlen(string); i++) {
      temp = string[i];
      if(temp > max) { max = temp; }
      if(temp < min) { min = temp; }
      sum += temp;
    }
    printf("max: %d min: %d, sum: %d \n", max, min, sum);
    return 0;
}