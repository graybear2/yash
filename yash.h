#ifndef __yash_h__
#define __yash_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "FatStack.h"

int getTokens(char**);
void execute(char**, int);
void pipeHandler(char***, int);
char INPUT[2000];
void waitForChildren(int);
static void sig_int(int);
static void sig_tstp(int);
static void sig_chld(int);
void jobHandler();
void fgHandler();
void bgHandler();
typedef struct{
    int8_t status;
    char* newIn;
    char* newOut;
    char* newErr;
} retStruct;

retStruct* parse(char**);
int status, interrupt;
pid_t pid, pid_ch1, pid_ch2;
volatile int waitStatus;
FatStack* stack;
#endif
