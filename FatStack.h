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
        pid_t* jobs;
} FatStack;

void init(FatStack*);
void pushPid(FatStack*, pid_t);
void pushCmd(FatStack*, char**);
int popPid(FatStack*);
int removeJob(FatStack*, pid_t);
void printJobs(FatStack*);

#endif