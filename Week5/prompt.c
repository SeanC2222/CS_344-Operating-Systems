#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

#include "prompt.h"

/*Function: promptUser()
 *Descriptions: Prints the shell prompt to stdout
 *Parameters: None
 *Pre-Conditions: None
 *Post-Conditions: Prints the prompt to stdout
 */
void promptUser(){
   if (!isatty(fileno(stdin))){
      //Don't print anything
   } else {
      printf(": ");
      fflush(stdout);
   }
   return;
}

/*Function: getUserInput(char*)
 *Descriptions: Gets user input from stdin and stores in command
 *Parameters:  char* command - buffer for user command to be stored
 *Pre-Conditions: command must be malloc'ed to size that accomodates input
 *Post-Conditions: command is populated by user input from stdin
 */
void getUserInput(char* command){
   size_t n;
   getline(&command, &n, stdin); 
   return;
}

/*Function: checkBackground()
 *Descriptions: Function that waits with WNOHANG for any background process that		might have finished. Prints out their exit status to terminal.
	        Return value is true if a process was found, false if not.
 *Parameters: None
 *Pre-Conditions: None
 *Post-Conditions: If a child is waiting to terminate, wait for it and return
		   exit status 
 */
int checkBackground(){
   int i, bstatus = 0;

   //Check if any child has changed state
   bstatus = waitpid(-1, &i, WNOHANG);

   //If a child has returned,
   if(bstatus > 0){

      //Update the user on which process returned
      printf("Background PID %i is done: ", bstatus);

      //If process did not exit normally,
      if(!WIFEXITED(i)){

	 //Update the user that it was terminated by a signal
	 printf("Terminated by signal %i\n", i);

      //Else process exited normally
      } else {
	 //Update the user on the exit value
	 printf("Exit value is %i.\n", i);
      }

      //A process ended, so return true
      return 1;
   } else {

      //A process did not end, so return false
      return 0;
   }
}
