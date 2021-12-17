#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include "./proc_data.h"
#include "./data-structures/circQueue.h"
#include "./data-structures/minHeap.h"

typedef short SCHEDULER_TYPE;
#define RR 0
#define HPF 1
#define SRTN 2

typedef short bool;
#define true 1
#define false 0

#define PAUSED 3
#define TERMINATED 2
#define RUNNING 1
#define WAITING 0

#define SHKEY 300

#define SHKEYPROC 231

///==============================
//don't mess with this variable//
int *shmaddr;                   //
struct process *shmaddrProcess; // this is the shared memory between currently running process and the scheduler
//===============================

char *log_file = "scheduler.log";
char *perf_file = "scheduler.perf";

int getClk()
{
    return *shmaddr;
}

/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/

void write_to_file(char *file_name, char *text)
{
    FILE *pFile;
    pFile = fopen(file_name, "a");

    fprintf(pFile, "%s\n", text);

    fclose(pFile);
}

void print_statistics(float Ex, float Ex2, int total_processes, float total_wait_time, int total_useful_time)
{
    //print statistics
    float mean_wta = Ex / total_processes;
    float std_dev_wta = sqrt(Ex2 / total_processes - (mean_wta * mean_wta));
    float mean_waiting_time = total_wait_time / total_processes;

    char avg_wta_message[25];
    char avg_waiting_time_message[25];
    char std_dev_wta_message[25];
    char cpu_utilization_message[25];
    sprintf(avg_wta_message, "Avg WTA = %.2f", mean_wta);
    sprintf(avg_waiting_time_message, "Avg Waiting = %.2f", mean_waiting_time);
    sprintf(std_dev_wta_message, "Std WTA = %.2f", std_dev_wta);
    sprintf(cpu_utilization_message, "CPU utilization = %.2f%%", (float)total_useful_time / (float)getClk() * 100);

    write_to_file(perf_file, cpu_utilization_message);
    write_to_file(perf_file, avg_wta_message);
    write_to_file(perf_file, avg_waiting_time_message);
    write_to_file(perf_file, std_dev_wta_message);
}

void initiate_process(struct process *picked_proc, int wait_time, float *total_wait_time)
{
    picked_proc->startTime = getClk();
    picked_proc->waitingTime += wait_time;
    *total_wait_time += wait_time;
    picked_proc->state = RUNNING;
}

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}

// Process shared memory init
void initShmProc()
{
    int shmid = shmget(SHKEYPROC, sizeof(struct process), 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The process shared memory not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEYPROC, sizeof(struct process), 0444);
    }
    shmaddrProcess = (struct process *)shmat(shmid, (void *)0, 0);
}

void setShmProc(struct process *newProc)
{

    memcpy(shmaddrProcess, newProc, sizeof(struct process));
};

struct process getProcess()
{
    return *shmaddrProcess;
}

void destroyShmProc(bool terminateAll)
{
    shmdt(shmaddrProcess);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}
