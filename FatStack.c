#include "FatStack.h"

void init(FatStack* stack){
    stack->size = 100; //100 is max
    stack->pidLen = 0;
    stack-> cmdLen = 0;
    stack->foreground = (pid_t*) malloc(sizeof(pid_t)*2);
    stack->foreground[0] = -1;
    stack->foreground[1] = -1;
    stack->fgNum = 0;
    stack->jobs = malloc(sizeof(pid_t)*100);
    stack->commands = (char***) malloc(sizeof(char**)*100);
    for(int i=0; i<100; i++)
        stack->commands[i] = (char**) malloc(sizeof(char*));
}

void pushPid(FatStack* stack, pid_t pid){
    if(stack->pidLen >= stack->size){
        printf("Exceeded max job amount");
        exit(1);
    }
    stack->jobs[stack->pidLen] = pid;
    stack->pidLen++; 
}

void pushCmd(FatStack* stack, char** command){
    if(stack->cmdLen >= stack->size){
        printf("Exceeded max job amount");
        exit(1);
    }

    int i=0;
    while(command[i] != NULL){
        stack->commands[stack->cmdLen][i] = strdup(command[i]);
        i++;
    }

    stack->commands[stack->cmdLen][i] = malloc(1);
    stack->commands[stack->cmdLen][i] = (char*) NULL;
    stack->cmdLen++;
}

int popPid(FatStack* stack){
    return stack->jobs[stack->pidLen-1];
}

int removeJob(FatStack* stack, pid_t pid){
    int found = 0;
    int index = 0;
    for(int i=0; i<stack->pidLen; i++){
        if(stack->jobs[i] == pid){
            found = 1;
            index = i;
        }
    }
    if(!found)
        return -1;
    for(int i=index; i<stack->pidLen-1; i++){
        stack->jobs[i] = stack->jobs[i+1];
    }
    stack->pidLen--;
    for(int i=index; i<stack->cmdLen-1; i++){
        stack->commands[i] = stack->commands[i+1];
    }
    stack->cmdLen--;
    return 1;
}

void printJobs(FatStack* stack){
    for(int i=0; i<stack->cmdLen; i++){
            int k=0;
            printf("Job %d: ", i);
            while(stack->commands[i][k] != NULL){
                printf("%s ", stack->commands[i][k]);
                k++;
            }
            printf(", PID: %d\n", stack->jobs[i]);
    }
}
