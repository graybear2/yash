#include "yash.h"

int main(int argc, char **argv){
    stack = (FatStack*) malloc(sizeof(FatStack)); 
    init(stack);
    if(signal(SIGINT, sig_int) == SIG_ERR)
        perror("signal(SIGINT) error");
    if(signal(SIGTSTP, sig_tstp) == SIG_ERR)
        perror("signal(SIGTSTP) error");
    if(signal(SIGCHLD, sig_chld) == SIG_ERR)
        perror("signal(SIGCHLD) error");

    printf("# ");
    while(fgets(INPUT, 2000, stdin)){   //max of 2000 characters
        char* TOKENS[1000]; 
        char** pipeCmd[2];
        int handle = 0;

        handle = (getTokens(TOKENS));
        
        if(handle & 1){
            pipeCmd[0] = &TOKENS[0];
            int i=0;
            while(TOKENS[i][0] != '|'){
                pipeCmd[1] = &TOKENS[i+2];
                i++;
            }
            TOKENS[i] = (char*) NULL; //change the pipe symbol to a null

            pipeHandler(pipeCmd, handle & 2);
        }
        else{
            if(!strcmp(TOKENS[0], strdup("fg")))
                fgHandler();
            else if(!strcmp(TOKENS[0], strdup("bg")))
                bgHandler();
            else
                execute(TOKENS, handle & 2);
        }

        stack->foreground[0] = -1;
        printf("# ");
    }
    return 0;
}

int getTokens(char** input){
    int retVal = 0;
    char* token = strtok(INPUT, "\n ");
    int i;
    for(i=0; token!=NULL; i++){
        if(token[0] == '|')
            retVal |= 1;
        if(token[0] == '&')
            retVal |= 2;
        input[i] = strdup(token);
        token = strtok(NULL, "\n ");
    }

    input[i] = malloc(1);
    input[i] = (char*) NULL;

    if(strcmp(input[0], strdup("jobs")))
        if(strcmp(input[0], strdup("fg")))
            if(strcmp(input[0], strdup("bg")))
                pushCmd(stack, input);

    return retVal;
}

retStruct* parse(char** TOKENS){
    int i=0;
    retStruct* retVal = malloc(sizeof(retStruct));
    retVal->status = 0;
    retVal->newIn = NULL;
    retVal->newOut = NULL;
    retVal->newErr = NULL;
    while(TOKENS[i] != NULL){
        if(TOKENS[i][0] == '<'){
            retVal->status |= 4;
            retVal->newIn = TOKENS[i+1];
            TOKENS[i] = (char*) NULL;
        }
        else if(TOKENS[i][0] == '>'){
            retVal->status |= 2;
            retVal->newOut = TOKENS[i+1];
            TOKENS[i] = (char*) NULL;
        }
        else if(TOKENS[i][0] == '2'){
            if(TOKENS[i][1] == '>'){
                retVal->status |= 1;
                retVal->newErr = TOKENS[i+1];
                TOKENS[i] = (char*) NULL;
            }
        }
        else if(TOKENS[i][0] == '&'){
            TOKENS[i] = (char*) NULL;
        }

        i++;
    }

    return retVal;
}

void execute(char** TOKENS, int bg){
    pid_ch1 = fork();
    if(stack->cmdLen > stack->pidLen)
        pushPid1(stack, pid_ch1);
    if(pid_ch1 < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    } 
    else if(pid_ch1 > 0){
        //parent
        //if(signal(SIGINT, sig_int) == SIG_ERR)
        //    printf("signal(SIGINT - single) error");
        //if(signal(SIGTSTP, sig_tstp) == SIG_ERR)
        //    printf("signal(SIGTSTP - single) error");
        stack->foreground[0] = pid_ch1;
        stack->foreground[1] = -1;
        if(!bg)
            waitForChildren(1); 
    }
    else{ 
        //child 1
        setsid();
        //setpgid(pid_ch1, 0);
        retStruct* parseInfo = parse(TOKENS);
        if(parseInfo->status & 4){
            //FILE* fp;
            //if((fp = fopen(parseInfo->newIn, "r")) == NULL)
            //    fprintf(stderr, "can't open %s", parseInfo->newIn);
            freopen(parseInfo->newIn, "r", stdin);
        }
        if(parseInfo->status & 2){
            freopen(parseInfo->newOut, "w", stdout);
        }
        if(parseInfo->status & 1){
            freopen(parseInfo->newErr, "w", stderr);
        }
        if(!strcmp(TOKENS[0], strdup("jobs")))
            jobHandler();
        else if(!strcmp(TOKENS[0], strdup("fg")))
            fgHandler();
        else if(!strcmp(TOKENS[0], strdup("bg")))
            bgHandler();
        else if(execvp(TOKENS[0], TOKENS) < 0){
            perror("execvp");
            exit(1);
        }
    }
}

void pipeHandler(char*** stateStart, int bg){ 
    int pipefd[2]; 

    if(pipe(pipefd) == -1){
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_ch1 = fork();
    if(pid_ch1 < 0){
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid_ch1 > 0){
        // Parent
        //printf("pid_ch1: %d\n", pid_ch1);
        pid_ch2 = fork();
        if (pid_ch2 > 0){
            //Parent
            //printf("pid_ch2: %d\n", pid_ch2);
            //if(signal(SIGINT, sig_int) == SIG_ERR)
            //    printf("signal(SIGINT - pipe) error");
            //if(signal(SIGTSTP, sig_tstp) == SIG_ERR)
            //    printf("signal(SIGTSTP - pipe) error");
            stack->foreground[0] = pid_ch1;
            stack->foreground[1] = pid_ch2;
            if(stack->cmdLen > stack->pidLen)
                pushPid2(stack, pid_ch1, pid_ch2);
            close(pipefd[0]);
            close(pipefd[1]);
            if(!bg)
                waitForChildren(2);            
        }
        else {
            //Child 2
            int status;
            waitpid(pid_ch1, &status, WUNTRACED | WCONTINUED);
            setpgid(0,pid_ch1); //child2 joins the group whose group id is same as child1's pid
            close(pipefd[1]); // close the write end
            retStruct* parseInfo = parse(stateStart[1]);
            if(parseInfo->status & 2){
                freopen(parseInfo->newOut, "w", stdout);
            }
            if(parseInfo->status & 1){
                freopen(parseInfo->newErr, "w", stderr);
            }

            dup2(pipefd[0],STDIN_FILENO);
            
            if(!strcmp(stateStart[1][0], strdup("jobs")))
                jobHandler();
            else if(!strcmp(stateStart[0][0], strdup("fg")))
                fgHandler();
            else if(!strcmp(stateStart[0][0], strdup("bg")))
                bgHandler();
            else if(execvp(stateStart[1][0], stateStart[1]) < 0){ // runs the second half of the pipe
                perror("execvp");
                exit(1);
            }
            //execute(stateStart[1]);
        }
    } 
    else {
        // Child 1
        setsid(); // child 1 creates a new session and a new group and becomes leader -
                  //   group id is same as his pid: pid_ch1
        //setpgid(pid_ch1, 0);
        close(pipefd[0]); // close the read end
        retStruct* parseInfo = parse(stateStart[0]);
        if(parseInfo->status & 4){ 
            freopen(parseInfo->newIn, "r", stdin);
        } 
        if(parseInfo->status & 1){
            freopen(parseInfo->newErr, "w", stderr);
        }

        dup2(pipefd[1],STDOUT_FILENO);
        
        if(!strcmp(stateStart[0][0], strdup("jobs")))
            jobHandler();
        else if(!strcmp(stateStart[0][0], strdup("fg")))
            fgHandler();
        else if(!strcmp(stateStart[0][0], strdup("bg")))
            bgHandler();
        else if(execvp(stateStart[0][0], stateStart[0]) < 0){  // first half of the pipe
            perror("execvp");
            exit(1);
        }
    }
}

void waitForChildren(int numChild){
    int count = 0;
    stack->fgNum = numChild;
    //while (stack->fgNum > 0) {
        
        while(count < numChild){
            pid = waitpid(-1, &status, WUNTRACED | WCONTINUED); //TODO
            //printf("pid %d and %d in foreground\n", stack->foreground[0], stack->foreground[1]);
            int background = (pid != stack->foreground[0] && pid != stack->foreground[1]);
            if(pid == -1)
            {
               //perror("waitpid");
               //exit(EXIT_FAILURE);
               //printf("no waitForChildren\n");
               //exit(1);
               background = 0;
            }
            if(background){
                //printf("background process died: %d\n", pid);
            }
            // wait does not take options:
            //    waitpid(-1,&status,0) is same as wait(&status)
            // with no options waitpid wait only for terminated child processes
            // with options we can specify what other changes in the child's status
            // we can respond to. Here we are saying we want to also know if the child
            // has been stopped (WUNTRACED) or continued (WCONTINUED)
            //printf("status: %d\n", status); 
            if (WIFEXITED(status)) {
              //printf("child %d exited, status=%d\n", pid, WEXITSTATUS(status));
              if(!background)
                count++;
              removeJob(stack, pid);
              //printf("count: %d\n", count);
            } 
            else if (WIFSIGNALED(status)) {
                //printf("child %d killed by signal %d\n", pid, WTERMSIG(status));
                if(!background)
                    count++;
                removeJob(stack, pid);
            } 
            else if (WIFSTOPPED(status)) {
                //printf("%d pipe stopped by signal %d\n", pid,WSTOPSIG(status));
                if(!background)
                    count++;
                printStopJob(stack);
                //printf("Sending CONT to %d\n", pid);
                //sleep(4); //sleep for 4 seconds before sending CONT
                //kill(pid,SIGCONT);
            } 
            else if (WIFCONTINUED(status)) {
                //printf("Continuing %d\n",pid);
            } 
        }
}

void jobHandler(){
    printJobs(stack);
    exit(0);
}

void fgHandler(){
    int numChildren;
    if(stack->cmdLen == 0){
        printf("yash: fg: current: no such job\n");
    }
    else{
        numChildren = 1;
        kill(popPid(stack, 0), SIGCONT);
        stack->foreground[0] = popPid(stack, 0);
        if(popPid(stack, 1) != -1){
            kill(popPid(stack, 1), SIGCONT);
            stack->foreground[1] = popPid(stack, 1);
            numChildren = 2;
        }
        waitForChildren(numChildren);
    }
}

void bgHandler(){
    if(stack->cmdLen == 0){
        printf("No jobs in background\n");
    }
    else{ 
        kill(popPid(stack, 0), SIGCONT);
        if(popPid(stack, 1) != -1)
            kill(popPid(stack, 1), SIGCONT);
        waitForChildren(2);
    }
}

static void sig_int(int signo) {
    if(stack->foreground[0] != -1){
        //printf("Sending SIGINT to group:%d\n",stack->foreground[0]); // group id is pid of first in pipeline
        interrupt = 1;
        kill(stack->foreground[0], SIGINT);
        if(stack->foreground[1] != -1)
            kill(stack->foreground[1], SIGINT);
    }
}
static void sig_tstp(int signo) {
    if(stack->foreground[0] != -1){
        //printf("Sending SIGTSTP to group:%d\n",pid_ch1); // group id is pid of first in pipeline
        interrupt = 1;
        kill(stack->foreground[0], SIGTSTP);
        if(stack->foreground[1] != -1)
            kill(stack->foreground[1], SIGTSTP);
    }
}

static void sig_chld(int signo){
    //printf("child received some signal\n");
}
