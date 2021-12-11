#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char *argv[])
{
    initClk();

    // get clk, running proc from shared memory
    int start_time = getClk();

    //TODO it needs to get the remaining time from somewhere
    remainingtime = atoi(argv[1]);
    pid_t scheduler_pid = atoi(argv[2]);
    SCHEDULER_TYPE sched_type = atoi(argv[3]);
    int pid = atoi(argv[4]);

    char log_message[100];
    sprintf(log_message, "Process %d started at %d, remaining time %d", pid, start_time, remainingtime);
    write_to_file("./proc.txt", log_message);

    switch (sched_type)
    {
    case RR:
        /* code */
        break;
    case SRTN:
        /* code */
        break;
    case HPF:

        while (remainingtime > 0)
        {
            int current_time = getClk();
            int time_elapsed = current_time - start_time;

            if (time_elapsed == remainingtime)
            {
                // process is finished
                break;
            }
            else if (time_elapsed > remainingtime)
            {
                // log error
                break;
            }
        }
        break;
    }

    kill(scheduler_pid, SIGCONT);
    
    destroyClk(false);
    
    raise(SIGKILL);
    return 0;
}
