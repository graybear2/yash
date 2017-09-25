#ifndef __FatStack_H__
#define __FatStack_H__
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct{
        int size;
        int pidLen;
        int cmdLen;
        pid_t* foreground;
        volatile int fgNum;
        char*** commands;
        int* phraseLen;
        pid_t** jobs;
} FatStack;

void init(FatStack*);
void pushPid1(FatStack* stack, pid_t);
void pushPid2(FatStack* stack, pid_t, pid_t);
void pushCmd(FatStack*, char**);
int popPid(FatStack*, int);
int removeJob(FatStack*, pid_t);
void printJobs(FatStack*);
void printStopJob(FatStack*);

#endif
