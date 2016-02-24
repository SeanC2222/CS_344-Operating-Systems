#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <sys/types.h>
#include "tokenizedcommand.h"

int executeInputCommand(struct tokenizedCommand*);
int childProcess(struct tokenizedCommand*);
int parentProcess(struct tokenizedCommand*,pid_t);
int checkRedirects(struct tokenizedCommand*);
#endif
