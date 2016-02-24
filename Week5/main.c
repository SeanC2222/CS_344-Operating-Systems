#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>

#include "definitions.h"
#include "prompt.h"
#include "parser.h"
#include "executer.h"
#include "tokenizedcommand.h"

int performActions(char*,struct tokenizedCommand*,int);

/*Function:	  mySigHandler(int)
 *Description:	  Signal handler for ignoring signal and reprompting
 *Parameters:	  int sig - Signal number received by process
 *Pre-Conditions: Signal is sent to current process
 *Post-Conditinos:Signal is ignored, prompt is written and stopped process
		  restarts
 */
void mySigHandler(int sig){
   write(fileno(stdout), "\n: ", 3);
   return;
}

/*Function:	  main()
 *Descriptions:	  Main function, houses program loop
 *Parameters:	  None
 *Pre-Conditions: No arguments when run
 *Post-Conditions:Returns exit value from last command run (can be exit)
 */
int main(){

   //Command buffer that stores user input
   char* command;
   command = malloc(COMMAND_LINE_MAX * sizeof(char));

   //Build a sigaction to ignore signals
   struct sigaction act;
   //Set action to ignore
   act.sa_handler = &mySigHandler;
   //Set flag to restart system calls if signal is received during system call
   act.sa_flags = SA_RESTART;
   //Mask all incoming signals during sa_handler
   sigfillset(&act.sa_mask);

   //Set current process sigaction for SIGINT to act
   //(NOTE: ALL OTHER SIGNALS WORK AS NORMAL)
//   signal(SIGINT,mySigHandler);
   sigaction(SIGINT, &act, NULL);

   //Dynamically allocate and initialize to 0/NULL a tokenizedCommand
   struct tokenizedCommand* tC;
   tC = malloc(sizeof(struct tokenizedCommand));
   initializeTokenizedCommand(tC);

   
   int status = 0;
   do{
      resetTokenizedCommand(tC);
      sigaction(SIGINT, &act, NULL);

      //checkBackground() prints all background processes that have returned
      while(checkBackground());
      
      //Displays appropriate prompt
      promptUser();

      //Populates command with user input c-string
      getUserInput(command);

      //Changes space and '\n' to '\0' and points tC->token[i] at each token
      parseInput(command, tC);

      //Attempts to perform input command, returns exit code or encoded status
      status = performActions(command, tC, status);

   } while (strcmp(getTokenizedCommandIndex(tC,0),"quit")
         && strcmp(getTokenizedCommandIndex(tC,0), "exit"));

   //Free dynamically allocated memory
   free(command);
   freeTokenizedCommand(tC);

   return status;
}

/*Function:	  performActions(char*, struct tokenizedCommand*, int)
 *Descriptions:	  Interprets tC index 0, and performs appropriate action.
		  Executes built in command "status"
 *Parameters:	  char* command - tokenized c-string to prevent segfaults
		     if user inputs no arguments
		  struct tokenizedCommand* tC - Populated tokenizedCommand
		  int status - Exit code or encoded status for status command 
 *Pre-Conditions: tC should be initialized to expected values
 *Post-Conditions:Commands are executed as necessary, returns new exit code
		  or encoded status
 */
int performActions(char* command, struct tokenizedCommand* tC, int status){

   //If user doesn't enter any arguments (all whitespace characters)
   if(getTokenizedCommandIndex(tC,0) == NULL){
      setTokenizedCommandIndex(tC,0,command); //Prevents strcmp of NULL in

   //If tC contains the exit command, return desired exit status
   } else if (!strcmp(getTokenizedCommandIndex(tC,0), "exit")){

      //If exit command has at least 1 argument
      if(getTokenizedCommandIndex(tC,1) != NULL){

	 //And if that argument can be converted to an integer
	 if(strtol(getTokenizedCommandIndex(tC,1),NULL,10)){

	    //Return an (int) cast converted long integer
	    return (int)strtol(getTokenizedCommandIndex(tC,1),NULL,10);

	 //Else return -1 for false argument
	 } else {
	    return -1;
	 }

      //Else Command doesn't have any arguments, return 0
      } else {
	 return 0;
      }
   
   //If tC contains a comment, nothing to execute return 0
   } else if(getTokenizedCommandIndex(tC,0)[0] == '#'){
      return 0;

   //If tC contains "cd" command, change current directory
   } else if(!strcmp(getTokenizedCommandIndex(tC,0), "cd")){
      
      //cd command with no argument moves to HOME path
      if(getTokenizedCommandIndex(tC,1) == NULL){
	 return chdir(getenv("HOME"));
      }
      
      return chdir(getTokenizedCommandIndex(tC,1));

   //Else if user calls built-in function "status"   
   } else if (!strcmp(getTokenizedCommandIndex(tC,0), "status")){

      //If status is encoded as exit value print unencoded exit value
      if(WIFEXITED(status)){
	 printf("Exit value: %i\n", WEXITSTATUS(status));

      //Else if status is encoded as signal terminated, print signal
      } else if (WIFSIGNALED(status)){
	 printf("Terminated by signal: %i\n", WTERMSIG(status));

      //Else status is unencoded, print exit value
      } else {
	 printf("Exit value: %i\n", status);
      }

      //Reset status after successful execution
      status = 0;

   //Else user entered a command to attempt, status gets returned exit value
   //or encoded status
   } else {
      status = executeInputCommand(tC);
   }

   //Return status
   return status;
}

