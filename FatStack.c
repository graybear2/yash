#include "FatStack.h"

void init(FatStack* stack){
    stack->size = 100; //100 is max
    stack->pidLen = 0;
    stack-> cmdLen = 0;
    stack->foreground = (pid_t*) malloc(sizeof(pid_t)*2);
    stack->foreground[0] = -1;
    stack->foreground[1] = -1;
    stack->fgNum = 0;
    stack->phraseLen = (int*) malloc(sizeof(int)*100);
    stack->jobs = malloc(sizeof(pid_t*)*100);
    for(int i=0; i<100; i++){
        stack->jobs[i] = malloc(sizeof(pid_t)*2);
    }
    stack->commands = (char***) malloc(sizeof(char**)*100);
    for(int i=0; i<100; i++)
        stack->commands[i] = (char**) malloc(sizeof(char*));
}

void pushPid1(FatStack* stack, pid_t pid){
    if(stack->pidLen >= stack->size){
        printf("Exceeded max job amount");
        exit(1);
    }
    stack->jobs[stack->pidLen][0] = pid;
    stack->jobs[stack->pidLen][1] = -1;
    stack->pidLen++; 
}

void pushPid2(FatStack* stack, pid_t pid1, pid_t pid2){
    if(stack->pidLen >= stack->size){
        printf("Exceeded max job amount");
        exit(1);
    }
    stack->jobs[stack->pidLen][0] = pid1;
    stack->jobs[stack->pidLen][1] = pid2;
    stack->pidLen++;
}

void pushCmd(FatStack* stack, char** command){
    //printJobs(stack);
    // int k;
    // for(k=0; command[k] != NULL; k++)
    //     printf("%s ", command[k]);
    // printf("\n");

    if(stack->cmdLen >= stack->size){
        printf("Exceeded max job amount");
        exit(1);
    }

    int i=0;
    //printf("cmdLen: %d\n", stack->cmdLen);
    while(command[i] != NULL){
        stack->commands[stack->cmdLen][i] = strdup(command[i]);
        i++;
    }

    stack->commands[stack->cmdLen][i] = malloc(sizeof(char*));
    stack->commands[stack->cmdLen][i] = (char*) NULL;
    stack->phraseLen[stack->cmdLen] = i;
    stack->cmdLen++;
    //printJobs(stack);   
}

int popPid(FatStack* stack, int n){
    return stack->jobs[stack->pidLen-1][n];
}

int removeJob(FatStack* stack, pid_t pid){
    int found = 0;
    int index = 0;
    for(int i=0; i<stack->pidLen; i++){
        if(stack->jobs[i][0] == pid || stack->jobs[i][1] == pid){
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
        if(i+1 == stack->cmdLen)
            printf("[%d]-  Running                 ", i+1);
        else
            printf("[%d]+  Running                 ", i+1);
        for(int k=0; k<stack->phraseLen[i]; k++){
            printf("%s ", stack->commands[i][k]);
        }
        printf("\n");
    }
}
void printStopJob(FatStack* stack){
    printf("Stopped    ");
    for(int k=0; k<stack->phraseLen[stack->cmdLen-1]; k++){
        printf("%s ", stack->commands[stack->cmdLen-1][k]);
    }
    printf("\n");
}
