#include <stdlib.h>

#include "tokenizedcommand.h"
#include "definitions.h"

/*Function:	  initializeTokenizedCommand(struct tokenizedCommand*)
 *Description:	  Initializes tC to 0/NULL. Malloc's for token.  
 *Parameters:	  struct tokenizedCommand* tC - malloc'ed struct 
 *Pre-Conditions: tC should be unitialized (else see resetTokenizedCommand())
 *Post-Conditions:tC is initialized, and tC->token is intialized
 */
int initializeTokenizedCommand(struct tokenizedCommand* tC){
   int i;

   tC->builtin = 0;	//Set default to exec()
   tC->oRedirect = 0;	//Set default output redirection to no redirection
   tC->oFileName = NULL;//Set default to no file name
   tC->iRedirect = 0;	//Set default input redirection to no redirection
   tC->iFileName = NULL;//Set default to no file name
   tC->background = 0;	//Set default to parent waits

   //Allocate max amount of tokens possible
   tC->token = (char**)malloc(ARGUMENT_MAX * sizeof(char*));

   //If malloc fails return signal
   if(tC->token == NULL){
      return -1;
   }

   //Initialize tokens to NULL (execvp() needs NULL terminator)
   for(i = 0; i < ARGUMENT_MAX; i++){
      tC->token[i] = NULL;
   }
   
   return 0;
}

/*Function:	  resetTokenizedCommand(struct tokenizedCommand*)
 *Description:	  Resets all values in tC to 0/NULL
 *Parameters:	  struct tokenizedCommand* tC - malloc'ed struct 
 *Pre-Conditions: tC should be initialized
 *Post-Conditions:All values in tC are reset to 0/NULL
 */
void resetTokenizedCommand(struct tokenizedCommand* tC){
   int i;
   //Reset all indices of token to NULL
   for(i = 0; i < ARGUMENT_MAX; i++){
      tC->token[i] = NULL;
   }
   //Reset all internal flags to 0/NULL
   tC->builtin = 0;
   tC->oRedirect = 0;
   tC->oFileName = NULL;
   tC->iRedirect = 0;
   tC->iFileName = NULL;
   tC->background = 0;
   return;
}

/*Function:	  setTokenizedCommandIndex(struct tokenizedCommand*, int, char*)
 *Description:	  Sets pointer to c-string (tC->token[i]) to point to str
 *Parameters:	  struct tokenizedCommand* tC - malloc'ed struct 
		  int i - index to set 
		  char* str - address to point tC->token[i] to
 *Pre-Conditions: tC should be initialized
 *Post-Conditions:Sets the c-string at index i to str
 */
void setTokenizedCommandIndex(struct tokenizedCommand* tC, int i, char* str){
   tC->token[i] = str;
   return;
}

/*Function:	  getTokenizedCommandIndex(struct tokenizedCommand*, int)
 *Description:	  Gets pointer to c-string at index i (tC->token[i])
 *Parameters:	  struct tokenizedCommand* tC - malloc'ed struct 
 *Pre-Conditions: tC should be initialized
 *Post-Conditions:Returns the c-string at index i 
 */
char* getTokenizedCommandIndex(struct tokenizedCommand* tC, int ind){
   return tC->token[ind];
}

/*Function:	  freeTokenizedCommand(struct tokenizedCommand*)
 *Description:	  Frees tC->token, and tC
 *Parameters:	  struct tokenizedCommand* tC - malloc'ed struct 
 *Pre-Conditions: tC should be initialized
 *Post-Conditions:Memory malloc'ed for tC is freed. 
		  tC is set to NULL.
 */
void freeTokenizedCommand(struct tokenizedCommand* tC){
   free(tC->token);
   free(tC);
   tC = NULL;
   return;
}

/*Function:	  setTokenizedCommandORedirect(struct tokenizedCommand*, int)
 *Description:	  Sets tC->oRedirect bool to r 
 *Parameters:	  struct tokenizedCommand* tC - malloc'ed struct 
		  int r - bool to set oRedirect to
 *Pre-Conditions: tC should be initialized
 *Post-Conditions:tC->oRedirect is set to bool r
 */
int setTokenizedCommandORedirect(struct tokenizedCommand* tC, int r){
   tC->oRedirect = r;
   return tC->oRedirect;
}

/*Function:	  getTokenizedCommandORedirect(struct tokenizedCommand*)
 *Description:	  Returns tC->oRedirect 
 *Parameters:	  struct tokenizedCommand* tC - malloc'ed struct 
 *Pre-Conditions: tC should be initialized
 *Post-Conditions:Returns tC->oRedirect
 */
int getTokenizedCommandORedirect(struct tokenizedCommand* tC){
   return tC->oRedirect;
}

/*Function:	  setTokenizedCommandOFileName(struct tokenizedCommand*, char*)
 *Description:	  Sets tC->oFileName to str
 *Parameters:	  struct tokenizedCommand* tC - malloc'ed struct 
		  char* str - c-string containing the output file path 
 *Pre-Conditions: tC should be initialized
		  str should point to c-string containing valid file path
 *Post-Conditions:tC->oFileName is set to str
 */
char* setTokenizedCommandOFileName(struct tokenizedCommand* tC, char* str){
   tC->oFileName = str;
   return tC->oFileName;
}

/*Function:	  getTokenizedCommandOFileName(struct tokenizedCommand*)
 *Description:	  Returns currently stored output file name
 *Parameters:	  struct tokenizedCommand* tC - malloc'ed struct 
 *Pre-Conditions: tC should be initialized
 *Post-Conditions:Returns tC->oFileName
 */
char* getTokenizedCommandOFileName(struct tokenizedCommand* tC){
   return tC->oFileName;
}

/*Function:	  setTokenizedCommandIRedirect(struct tokenizedCommand*, int)
 *Description:	  Sets tC->iRedirect bool to r
 *Parameters:	  struct tokenizedCommand* tC - malloc'ed struct 
		  int r - bool to set iRedirect to
 *Pre-Conditions: tC should be initialized
 *Post-Conditions:tC->iRedirect is set to bool r
 */
int setTokenizedCommandIRedirect(struct tokenizedCommand* tC, int r){
   tC->iRedirect = r;
   return tC->iRedirect;
}

/*Function:	  getTokenizedCommandIRedirect(struct tokenizedCommand*)
 *Description:	  Returns tC->iRedirect 
 *Parameters:	  struct tokenizedCommand* tC - malloc'ed struct 
 *Pre-Conditions: tC should be initialized
 *Post-Conditions:Returns tC->iRedirect
 */
int getTokenizedCommandIRedirect(struct tokenizedCommand* tC){
   return tC->iRedirect;
}

/*Function:	  setTokenizedCommandIFileName(struct tokenizedCommand*, char*)
 *Description:	  Sets tC->iFileName to str
 *Parameters:	  struct tokenizedCommand* tC - malloc'ed struct 
		  char* str - c-string containing the input file path
 *Pre-Conditions: tC should be initialized
 *Post-Conditions:tC->iFileName is set to str
 */
char* setTokenizedCommandIFileName(struct tokenizedCommand* tC, char* str){
   tC->iFileName = str;
   return tC->iFileName;
}

/*Function:	  getTokenizedCommandIFileName(struct tokenizedCommand*)
 *Description:	  Returns currently stored input file name
 *Parameters:	  struct tokenizedCommand* tC - malloc'ed struct 
 *Pre-Conditions: tC should be initialized
 *Post-Conditions:Returns tC->iFileName
 */
char* getTokenizedCommandIFileName(struct tokenizedCommand* tC){
   return tC->iFileName;
}

/*Function:	  setTokenizedCommandBackground(struct tokenizedCommand*, int)
 *Description:	  Sets tC's background to bkg 
 *Parameters:	  struct tokenizedCommand* tC - malloc'ed struct
 *Pre-Conditions: tC should be initialized
 *Post-Conditions:
 */
int setTokenizedCommandBackground(struct tokenizedCommand* tC, int bkg){
   tC->background = bkg;
   return tC->background;
}

/*Function:	  getTokenizedCommandBackground(struct tokenizedCommand*)
 *Description:	  Returns tC's background flag
 *Parameters:	  struct tokenizedCommand* tC - malloc'ed struct
 *Pre-Conditions: tC should be initialized
 *Post-Conditions:background is returned to calling function
 */
int getTokenizedCommandBackground(struct tokenizedCommand* tC){
   return tC->background;
}
