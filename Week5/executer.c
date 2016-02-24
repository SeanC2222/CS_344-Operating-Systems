#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "executer.h"
#include "definitions.h"

/*Function:	  executeInputCommand(struct tokenizedCommand*)
 *Description: 	  Accepts a pointer to a tokenizedCommand and tries to execute
		  the command.
 *Parameters:	  struct tokenizedCommand* tC - Populated struct with command
		     information to try to execute.
 *Pre-Conditions: tC must be malloc'ed and assigned expected values
 *Post-Conditions:No changes are made to tC
		  If a child process is spawned in the foreground, smallsh
		     waits for exit value of process.
		  If a child process is spawned in the background, smallsh
		     will not wait for exit value. Process must be waited for
		     elsewhere.
		  If tC contains not built-in command arguments, a child process
		     is spawned and execvp is attempted. If successful child,
		     process will exit with appropriate exit value. If
		     unsuccessful, child process will exit with value 1.
 */
int executeInputCommand(struct tokenizedCommand* tC){

   //If here, command has been parsed and is a non-built-in command

   //Create child process
   pid_t pid = fork();

   //If pid == -1, fork failed (no child) and parent process will return to main
   if(pid == -1){
      perror("Fork failed.");
      return -1;

   //Child process runs childProcess()
   } else if(pid == 0){

      //childProcess() returns only if there's an error
      //exit the child process with the status of the error
      exit(childProcess(tC));

   //Parent process runs parentProcess()
   } else {

      //parentProcess() returns 0 if background or encoded signal as int
      return parentProcess(tC, pid);
   }
}

/*Function:	  parentProcess(struct tokenizedCommand*, pid_t)
 *Description: 	  The parent process won't wait for child and returns 0 if its a
		  background process, or waits for its child process and returns
		  the exit status.
 *Parameters:	  struct tokenizedCommand* tC - Populated struct with command
		     information to try to execute.
		  pid_t pid - copy of child process' pid
 *Pre-Conditions: tC must be malloc'ed and assigned expected values
		  Successful forking (i.e. pid > 0)
 *Post-Conditions:0 is always returned if tC's background flag is set
		  Encoded waitpid() status is returned otherwise
 */
int parentProcess(struct tokenizedCommand* tC, pid_t pid){

   //If tC's background flag is set update user and return 0
   if (getTokenizedCommandBackground(tC)){

      //Update user on background child PID 
      printf("Background PID is: %i\n", pid);
      return 0;

   //Else parent waits for child
   } else {

      //Initialize a sigaction to ignore SIGINT while child runs
      struct sigaction act;
      act.sa_handler = SIG_IGN;
      sigfillset(&act.sa_mask);
      act.sa_flags = 0;
      sigaction(SIGINT, &act, NULL);

     int status;

      //Wait for specific child process, and store encoded status
      waitpid(pid, &status, 0);

      //If child is terminated by signal, update the user
      if(WIFSIGNALED(status)){
	 printf("Terminated by signal: %i\n", WTERMSIG(status));
      }

      //Return encoded status
      return status;
   }
}

/*Function:	  childProcess(struct tokenizedCommand*)
 *Description: 	  The child process redirects its stdin && stdout if necessary
		  and attempts to execvp() based on tC's data. If successful
		  execvp() doesn't return (exit status is retrieved via
		  parentProcess() waitpit()), else returns error code.
 *Parameters:	  struct tokenizedCommand* tC - Populated struct with command
		     information to try to execute.
 *Pre-Conditions: tC must be malloc'ed and assigned expected values
		  Successful forking (i.e. only child process should run this)
 *Post-Conditions:If successful execvp() process exit()'s with exit code from
		     process.
		  If unsuccessful error code is returned.
 */
int childProcess(struct tokenizedCommand* tC){

   int status = 0;

   //Initialize a sigaction to ignore SIGINT
   struct sigaction act;
   act.sa_handler = SIG_IGN;
   sigfillset(&act.sa_mask);
   act.sa_flags = SA_RESTART;
   sigaction(SIGINT, &act, NULL);

   //If process runs in the foreground, set SIGINT actions to default
   if(!getTokenizedCommandBackground(tC)){
      act.sa_handler = SIG_DFL;
      sigaction(SIGINT, &act, NULL);
   }
   
   //If redirections are necessary, open/create files and redirect to them
      //(NOTE: This includes redirects due to background processes)
   status = checkRedirects(tC);

   //If checkRedirects returns an error flag, return that error flag
   if(status){
      return status;

   //Else attempt to execvp() the program
   } else {
      status = execvp(getTokenizedCommandIndex(tC,0),tC->token);
   }

   //If execvp() fails, assume the program doesn't exist (MAY NOT BE TRUE)
   printf("%s: no such file or directory\n", getTokenizedCommandIndex(tC,0));

   //Return exit value (execvp() only returns -1 if it fails to run
      //and the status macros (WIFEXITED, WIFSIGNALED) check the set bits in 
      //an integer so multiply by -1 to change from 11111111 to 000000001)
   return -1 * status;
}

/*Function:	  checkRedirects(struct tokenizedCommand*)
 *Description: 	  Checks tC for background and redirection flags and redirects
		  stdout and stdin as appropriate.
 *Parameters:	  struct tokenizedCommand* tC - Populated struct with command
		     information to try to execute.
 *Pre-Conditions: tC must be malloc'ed and assigned expected values
 *Post-Conditions:stdin and stdout of calling process may be redirected 
		  depending on tC flags.
 */
int checkRedirects(struct tokenizedCommand* tC){

   //If tC's background flag is set, set input and output to /dev/null
   if(getTokenizedCommandBackground(tC)){

      //Open /dev/null for read/write
      int nullIO = open("/dev/null", O_RDWR);

      //Dup nullIO file descriptor into stdin and stdout
      dup2(nullIO,fileno(stdin));
      dup2(nullIO,fileno(stdout));

      //Close nullIO (stdin open to /dev/null && stdout open to /dev/null
      close(nullIO);
   }

   //If tC's output redirection flag is set, set redirection
   //NOTE: Background commands original settings CAN be overwritten
   //	   i.e. background commands can be redirected to appropriate sources
   if(getTokenizedCommandORedirect(tC)){

      //Attempt to open output file, if not create output file
      int oF = open(getTokenizedCommandOFileName(tC), 
		    O_WRONLY | O_CREAT | O_CLOEXEC,   //Write-Only,Close on exec
		    S_IRUSR | S_IWUSR);		      //Read-Write permissions

      //If open() fails, update user and return error code
      if(oF == -1){
	 printf("smallsh: cannot open %s for output\n",
	        getTokenizedCommandOFileName(tC));
	 return 1;
      }

      //Dup oF file descriptor into stdout
      dup2(oF,fileno(stdout));

      //Close oF (stdout still open to oF)
      close(oF);
   }

   //If tC's input redirection flag is set, set redirection
   //NOTE: Background commands original settings CAN be overwritten
   //	   i.e. background commands can be redirected to appropriate sources
   if(getTokenizedCommandIRedirect(tC)){

      //Attempt to open input file
      int iF = open(getTokenizedCommandIFileName(tC),
		    O_RDONLY | O_CLOEXEC,	   //Read-Only, close on exec
		    S_IRUSR | S_IWUSR);		   //Read-Write Permissions

      //If open() fails, update user and return error code
      if(iF == -1){
	 printf("smallsh: cannot open %s for input\n",
	        getTokenizedCommandIFileName(tC));
	 return 1;
      }
      
      //Dup iF file descriptor into stdin
      dup2(iF,fileno(stdin));

      //Close iF (stdin still open to iF)
      close(iF);
   }

   return 0;
}
