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
    SCHEDULER_TYPE sched_type = argv[3] - '0';

    write_to_file("/home/hos/Coding/OS/Project/OS-Process-Scheduler/proc.log", "Process 1 started\n");

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

    
    destroyClk(false);
    
    raise(SIGKILL);
    return 0;
}
