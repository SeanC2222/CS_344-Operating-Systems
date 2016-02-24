#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[]){

   srand(time(NULL));

   if (argc > 2){
      printf("Too many command line arguments. Expected format:\n");
      printf("keygen [keylength] [< | >] [input file | output file]\n");
      return 1;
   } else if (argv[1] == NULL){
      printf("Expecting argument: keylength \nExpected format:\n");
      printf("keygen [keylengt] [< | >] [input file | output file]\n");
   }
   
   int i = 0, random = 0, keylength = 0;

   keylength = (int)strtol(argv[1],NULL, 10);

   char* newkey;
   newkey = malloc((keylength + 1) * sizeof(char));
   newkey[keylength] = '\0';

   for(i = 0; i < atoi(argv[1]); i++){
      random = rand() % 27 + 65;
      if(random == 91){
	 newkey[i] = ' ';
      } else {
	 newkey[i] = random;
      }
   }

   printf("%s", newkey);

   free(newkey);

   return 0;

}
