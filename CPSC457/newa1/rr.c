#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "main.h"
#include "queue.c"
#define buffer 64
#define numAttr 4
#define expectedLength 16
#define pid_column 0
#define arr_column 1
#define tim_column 2
#define bur_column 3

struct totalProcess* createTotalProcess(int pid) {

    struct totalProcess* newTotalProcess = malloc(sizeof(struct totalProcess));

    newTotalProcess->pid = pid;
    newTotalProcess->arrive = -1;
    newTotalProcess->burst = 0;
    newTotalProcess->start = -1;
    newTotalProcess->finish = 0;
    newTotalProcess->wait = 0;
    newTotalProcess->turnaround = 0;
    newTotalProcess->response = -1;

    return newTotalProcess;
}

void updateTotal(struct totalProcess* someTotal, int arrive, int burst, int start, int finish, int response) {


    if (someTotal->arrive == -1) {
        someTotal->arrive = arrive;
    }
    
    if (someTotal->start == -1) {
        someTotal->start = start;
    }

    someTotal->burst += burst;

    someTotal->finish = finish;

    // this has to be updated to account for processes being sat on the wait for much longer, will almost certainly involve using
    // the previous end time 
    someTotal->wait = finish - arrive - burst;

    someTotal->turnaround = finish - arrive;

    if ((someTotal->response == -1) && (response > 0)) {
        someTotal->response = response - arrive;
    }

}

int subtractZeroFloor(int minuend, int subtrahend) {

    if (minuend - subtrahend < 0) {
        return 0;
    }

    return (minuend - subtrahend);
}

int letInProcesses(struct queue* someQueue, struct process** procArray, int currentTime, int currentIndex, int length) {

    int i = currentIndex;
    bool loop = true;
    while (loop) {

        if (i >= length) {
            loop = false;
        }

        else if (procArray[i]->arrival < currentTime) {
            enqueue(someQueue, procArray[i]); 
            i++;
        }
        
        else {
            loop = false;
        }
    }

    return i;

}

struct process* createProcess(char* pid, char* arrival, char* time, char* burst) {

    struct process* newProcess = malloc(sizeof(struct process));

    newProcess->pid = atoi(pid);
    newProcess->arrival = atoi(arrival);
    newProcess->timeTilFirstResp = atoi(time);
    newProcess->burstLength = atoi(burst);
    newProcess->timeRemaining = newProcess->burstLength;
    newProcess->hasResponded = false;

    return newProcess;
}

void rr(struct process** procArray, int length, int quantum) {

    // setting up a queue for processes still active, and setting up another one for processes that are completed
    struct queue* procQueue = createQueue();
    struct queue* completedQueue = createQueue();
    struct process* currentProc = (struct process*) malloc(sizeof(struct process));

    procQueue->head = NULL;

    // all of these are in milliseconds
    char* firstLine = "Id, Arrival, Burst, Start, Finish, Wait, Turnaround, Response Time\n";
    char* standard =  "%3d, %7d, %5d, %5d, %6d, %4d, %10d, %13d\n";
    char* finalThree ="Average waiting time: %5.2f ms\nAverage turnaround time: %5.2f ms\nAverage response time: %5.2f ms\n";
    int i = 0;

    // listing off a bunch of the vars to be printed    
    int id, arrival, burst, start, finish, wait, turnaround, respTime;
    // defining a bunch of the values to determine averages
    double totalWaitingTime, totalTurnTime, totalRespTime;

    // initializing the queue
    enqueue(procQueue, procArray[0]);
    // setting the current time to the value of the arrival of the first process
    int currentTime = procArray[0]->arrival;

    int numUniqueProcs = 50;

    struct totalProcess* totalsArray[50];
    for (i = 0; i < numUniqueProcs; i++) {
        totalsArray[i] = createTotalProcess(i+1);
    }

    // printing the initial sequence
    printSequence(procArray, length);
    printf("\n");

    // printing the first line of the table
    printf(firstLine);

    i = 0;

    while (!isEmpty(procQueue)) {

        currentProc = dequeue(procQueue);

        id = currentProc->pid; arrival = currentProc->arrival; burst = quantum;
        start = currentTime, respTime = -1;
        
        //start = currentTime; wait = start + burst; turnaround = quantum; respTime = 0;
        
        if (currentProc->timeRemaining - quantum > 0) {
            currentTime += quantum;
            i = letInProcesses(procQueue, procArray, currentTime, i, length);       
            enqueue(procQueue, currentProc);
            currentProc->timeTilFirstResp -= quantum; 
            currentProc->timeRemaining -= quantum;
        }

        else  {
            currentTime += currentProc->timeRemaining;
            burst = currentProc->timeRemaining;
            i = letInProcesses(procQueue, procArray, currentTime, i, length);
            currentProc->timeTilFirstResp -= currentProc->timeRemaining;   
            currentProc->timeRemaining = 0;
            enqueue(completedQueue, currentProc);
        }

        if (currentProc->timeTilFirstResp <= 0) {
            respTime = currentTime + currentProc->timeTilFirstResp;
            currentProc->hasResponded = true;
        }

        finish = currentTime;

        updateTotal(totalsArray[id - 1], arrival, burst, start, finish, respTime);
    }

    struct totalProcess* currentTotal = (struct totalProcess*) malloc(sizeof(struct totalProcess));
    for (i = 0; i < numUniqueProcs; i++) {
        // printing off a new column
        currentTotal = totalsArray[i];
        // updating the totals
        totalWaitingTime += currentTotal->wait; totalTurnTime += currentTotal->turnaround; totalRespTime += currentTotal->response;
        printf(standard, currentTotal->pid, currentTotal->arrive, currentTotal->burst, currentTotal->start, currentTotal->finish, currentTotal->wait, currentTotal->turnaround, currentTotal->response);
    }

    // converting these to the averages, ought to update the variable names
    totalWaitingTime /= numUniqueProcs; totalTurnTime /= numUniqueProcs; totalRespTime /= numUniqueProcs;

    printf(finalThree, totalWaitingTime, totalTurnTime, totalRespTime);
}

int main() {

    // most crucial thing right now is to get the number of lines in an input file
    // PROCESSES ARE BEING READ IN CORRECTLY 
    // maybe use feof to determine end of file

    // https://stackoverflow.com/questions/12911299/read-csv-file-in-c
    // perhaps change to another name otherwise it may not work properly when TA has to run[]
    
    // actual input file
    FILE* inputCSV = fopen("input.csv", "r");
    // array of pointers to processes
    struct process* processArray[1000];
    // ie an array to temporarily hold each string before it is converted to an 
    char inputString[buffer];
    // array to hold the strings from cycler, which are then passed on the createProcess
    char holderArray[numAttr][expectedLength]; 
    // an array to run for as many lines are in the input file
    int i = 0;
    // fgets seems to divy it up by \n anyways, run a for loop here
    while (i < 1001) {
        
        // reads in lines from csv line by line
        fgets(inputString, buffer, inputCSV);
        // using strtok to cycle through the various process attributes
        strcpy(holderArray[pid_column], strtok(inputString, ","));
        strcpy(holderArray[arr_column], strtok(NULL, ","));
        strcpy(holderArray[tim_column], strtok(NULL, ","));
        strcpy(holderArray[bur_column], strtok(NULL, ","));

        // adds them provided we are past the first line, so the text won't be added as an entry
        if (i > 0) {
            processArray[i - 1] = createProcess(holderArray[pid_column], holderArray[arr_column], holderArray[tim_column], holderArray[bur_column]);
        }

        // continue on through the lines
        i++;
    }

    // closing the input csv
    fclose(inputCSV);

    // working well enough
    //fcfs(processArray, 1000);

    rr(processArray, 1000, 10);

    return 0;
}