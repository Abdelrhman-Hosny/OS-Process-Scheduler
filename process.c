#include "headers.h"

void handle_new_proc_arrival(int);

int main(int agrc, char *argv[])
{
    initClk();
    signal(SIGINT, handle_new_proc_arrival);

    // get clk, running proc from shared memory
    const int start_time = getClk();

    char log3[200];
    sprintf(log3, "current start time is %d ", start_time);
    write_to_file("proc.txt", log3);
    //TODO it needs to get the remaining time from somewhere
    int remainingtime = atoi(argv[1]);
    pid_t scheduler_pid = atoi(argv[2]);
    SCHEDULER_TYPE sched_type = atoi(argv[3]);
    int pid = atoi(argv[4]);

    char log_message[100];
    sprintf(log_message, "::Process %d started at %d, remaining time %d and my id is: %d", pid, start_time, remainingtime,getpid());
    write_to_file("./proc.txt", log_message);

    int i = 0;
    while (remainingtime > 0)
    {

        
        int current_time = getClk();

        int time_elapsed = current_time - start_time;

        if (time_elapsed == remainingtime)
        {
            char log_message2[100];
            sprintf(log_message2, "Current time is: %d remaining time is: %d time elapsed is: %d  start time is %d", current_time, remainingtime, time_elapsed, start_time);
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
    char log_message2[100];
    sprintf(log_message2, "recieved interrupt from scheduler at %d ", getClk());
    write_to_file("proc.txt", log_message2);
    destroyClk(false);
    signal(SIGUSR1, handle_new_proc_arrival);
    exit(0);
}
