#ifndef TOKENIZEDCOMMAND_H
#define TOKENIZEDCOMMAND_H

struct tokenizedCommand{//Expected values shown below:
      char** token;	// Valid address is command/arg, NULL is ignored
      int builtin;	// 1 built in command, 0 pass to executer
      int oRedirect;	// 1 output redirected, 0 no redirection
      char* oFileName;	// Valid address is file path, NULL means no file
      int iRedirect;	// 1 input redirected, 0 no redirection 
      char* iFileName;	// Valid address is file path, NULL means no file
      int background;	// 1 run in background (no wait), 0 parent waits
};


int initializeTokenizedCommand(struct tokenizedCommand*);

void resetTokenizedCommand(struct tokenizedCommand*);

void setTokenizedCommandIndex(struct tokenizedCommand*, int, char*);
char* getTokenizedCommandIndex(struct tokenizedCommand*, int);

void freeTokenizedCommand(struct tokenizedCommand*);

int setTokenizedCommandORedirect(struct tokenizedCommand*, int);
int getTokenizedCommandORedirect(struct tokenizedCommand*);

char* setTokenizedCommandOFileName(struct tokenizedCommand*, char*);
char* getTokenizedCommandOFileName(struct tokenizedCommand*);

int setTokenizedCommandIRedirect(struct tokenizedCommand*, int);
int getTokenizedCommandIRedirect(struct tokenizedCommand*);

char* setTokenizedCommandIFileName(struct tokenizedCommand*, char*);
char* getTokenizedCommandIFileName(struct tokenizedCommand*);

int setTokenizedCommandBackground(struct tokenizedCommand*, int);
int getTokenizedCommandBackground(struct tokenizedCommand*);

#endif
