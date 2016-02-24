#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define ARGUMENT_MAX 512
#define COMMAND_LINE_MAX 2048

//declaration in tokenizedcommand.h
/*
struct tokenizedCommand{//Expected values shown below:
      char** token;	// Valid address is command/arg, NULL is ignored
      int builtin;	// 1 built in command, 0 pass to executer
      int oRedirect;	// 1 output redirected, 0 no redirection
      char* oFileName;	// Valid address is file path, NULL means no file
      int iRedirect;	// 1 input redirected, 0 no redirection 
      char* iFileName;	// Valid address is file path, NULL means no file
      int background;	// 1 run in background (no wait), 0 parent waits
};
*/

#endif
