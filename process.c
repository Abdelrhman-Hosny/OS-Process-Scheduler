#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

void handle_new_proc_arrival(int);

int main(int agrc, char *argv[])
{
    initClk();
    signal(SIGUSR1, handle_new_proc_arrival);

    // get clk, running proc from shared memory
    int start_time = getClk();

    //TODO it needs to get the remaining time from somewhere
    remainingtime = atoi(argv[1]);
    pid_t scheduler_pid = atoi(argv[2]);
    SCHEDULER_TYPE sched_type = atoi(argv[3]);
    int pid = atoi(argv[4]);

    char log_message[100];
    sprintf(log_message, "::Process %d started at %d, remaining time %d", pid, start_time, remainingtime);
    write_to_file("./proc.txt", log_message);

    while (remainingtime > 0)
    {
        int current_time = getClk();
        int time_elapsed = current_time - start_time;

        if (time_elapsed == remainingtime)
        {
            char log_message2[100];
            sprintf(log_message2, "%d ", time_elapsed);
            write_to_file("proc.txt", log_message2);
            // process is finished
            break;
        }
        else if (time_elapsed > remainingtime)
        {
            char log_message2[100];
            sprintf(log_message2, "%d ", time_elapsed);
            write_to_file("proc.txt", log_message2);
            // log error
            break;
        }
    }

    destroyClk(false);
    return pid;
}

void handle_new_proc_arrival(int signum)
{
    destroyClk(false);
    exit(0);
}
