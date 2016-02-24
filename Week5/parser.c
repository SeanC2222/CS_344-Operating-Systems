#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "definitions.h"

/*Function:	  parseInput(char*, struct tokenizedCommand*)
 *Descriptions:	  Takes a c-string, and uses strtok to put null terminators
		  in white space characters. Sets up a tokenizedCommand struct
		  to point to each of the individual tokens so that execvp() can
		  be called using the struct's array of c-strings.
 *Parameters:	  char* command - c-string to be parsed
		  struct tokenizedCommand* tC - Pointer to a struct that will
		     contain the array of c-string pointers and state flags
 *Pre-Conditions: command must be malloc'ed and point to NULL, or not junk
		  tC must be malloc'ed and initialized to 0/NULL
 *Post-Conditions:All spaces and '\n' in command will be replaced by '\0'
		  tC's array will contain pointers to the newly terminated
		     c-string tokens of command
		  tC's state flags will be set, and FileNames set
 */
void parseInput(char* command, struct tokenizedCommand* tC){ 

   //Pointer used to increment through command
   char* pParser;

   //Initialize strtok, and point to first token
   pParser = strtok(command, " \n");

   //Set tC[0] to first token
   setTokenizedCommandIndex(tC,0,pParser);

   //While strtok returns a valid address, keep storing tokens
   int i = 1;
   while(pParser != NULL){

      //pParser gets next token if there, if not pParser gets NULL
      pParser = strtok(NULL, " \n");

      //Must break if pParser is NULL because strcmp will segfault
      if(pParser == NULL){
	 break;
      }

      //If current token is ">", set oRedirect flag 
      if(!strcmp(pParser,">")){
	 setTokenizedCommandORedirect(tC, 1);

      //Else if current token is "<" set iRedirect flag
      } else if (!strcmp(pParser,"<")){
	 setTokenizedCommandIRedirect(tC, 1);

      //Else if current token is "&" set background flag
      } else if (!strcmp(pParser,"&")){
	 setTokenizedCommandBackground(tC, 1);
	 break;	  //Must be last word in command line

      //Else set current token in tokenizedCommand to proper index
      } else {
	 //ALWAYS set tC[i] to equal pParser
	 setTokenizedCommandIndex(tC,i,pParser);

	 //If oRedirect is set but has no FileName set FileName to current index
	 if(getTokenizedCommandOFileName(tC) == NULL
	 && getTokenizedCommandORedirect(tC) == 1){

	    //Set output file name in struct to address at tC[i]
	    setTokenizedCommandOFileName(tC,getTokenizedCommandIndex(tC,i));

	    //Remove output file name from array (not a command argument)
	    setTokenizedCommandIndex(tC,i,NULL);
	    i--; //Negates later increment from successful token

	 //If oRedirect is set but has no FileName set FileName to current index
	 } else if (getTokenizedCommandIFileName(tC) == NULL
	 && getTokenizedCommandIRedirect(tC) == 1){

	    //Set input file name in struct to address at tC[i]
	    setTokenizedCommandIFileName(tC,getTokenizedCommandIndex(tC,i));

	    //Remove input file name from array (not a command argument)
	    setTokenizedCommandIndex(tC,i,NULL);
	    i--; //Negates later increment from successful token
	 }

	 //Increment index to store next token
	 i++;
      }
   }
   return;
}

