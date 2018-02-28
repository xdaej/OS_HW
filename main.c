//
//  main.c
//  OS_Problem-1
//
//  Created by Kevin Lin on 2/27/18.
//  Copyright © 2018 Kevin Lin. All rights reserved.
//
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Structures to define the process tree */
struct processInfo
{
    char processName; /* A, B, C, D */
    int numChild;
    char children[5];
    int retValue;
};

/* Define the process tree */
struct processInfo process[] = {
    { 'A', 2, {'B', 'C'}, 5},
    { 'B', 1, {'D'}, 9 },
    { 'C', 0, { }, 8 },
    { 'D', 0, { }, 12 },
};

char myId = 'A'; /* my process id */
int retVal;

void processfunction()
{
    int i, j;
    int n = sizeof(process) / sizeof(struct processInfo);
    
    printf("Started process %c, pid=%d\n", myId, getpid());
    
    /* Get the children details */
    for(i = 0; i < n; i++)
    {
        if(myId == process[i].processName) /* process details found */
            break;
    }
    if(i < n) /* entry found */
    {
        /* update return value */
        retVal = process[i].retValue;
        /* create forked children */
        pid_t pids[5];
        for(j = 0; j < process[i].numChild; j++)
        {
            pids[j] = fork();
            if(pids[j] < 0)
            {
                printf("Process %c, pid=%d: fork failed\n", myId, getpid());
            }
            if(pids[j] == 0)
            {
                /* child process */
                /* 1. update id */
                myId = process[i].children[j];
                /* 2. call the processfunction, and return */
                processfunction();
                return;
            }
            else
            {
                printf("Process %c, pid=%d: Forked %c, pid=%d\n", myId, getpid(), process[i].children[j], pids[j]);
            }
        }
        /* Children forked, wait for children to end */
        printf("Process %c, pid=%d: Waiting for children to end\n", myId, getpid());
        for(j = 0; j < process[i].numChild; j++)
        {
            int status;
            if(pids[j] > 0) /* child was successfully forked */
            {
                waitpid(pids[j], &status, 0);
                printf("Process %c, pid=%d: Child exited with status %d\n", myId, getpid(), WEXITSTATUS(status));
            }
        }
    }
    
    /* Sleep for sometime */
    sleep(10);
    printf("Process %c, pid=%d: ending process\n", myId, getpid());
}

int main()
{
    processfunction();
    return retVal;
}