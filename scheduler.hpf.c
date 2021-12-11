#include "headers.h"

struct msgbuffer
{
    long mtype;
    struct process proc;
};
int start_time;
void sig_child_handler(int signum);

int main(int argc, char *argv[])
{

    initClk();

    signal(SIGCHLD, sig_child_handler);

    // init message queue
    key_t key = ftok("./clk.c", 'a');
    int msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1)
    {
        perror("msgget");
        return 1;
    }

    struct MinHeap heap = initializeMinHeap();

    while (1)
    {

        // while (1)
        // {
            struct msgbuffer msg;
            int rec_val = msgrcv(msgid, (struct msgbuffer *)&msg, sizeof(msg.proc), 0, !IPC_NOWAIT);

            if (rec_val == -1)
            {
                // message queue was empty
                break;
            }
            else
            {
                // add process to heap
                struct process proc = msg.proc;
                push_heap(&heap, proc, proc.priority);
            }
        // }

        struct process picked_proc = peek_heap(&heap);
        
        if (picked_proc.arrivalTime != -1)
        {
            // Queue is not empty
            // After the schedule picks the process
            // it will be executed

            start_time = getClk();

            int pid = fork();

            if (pid == -1)
                perror("error in fork");

            else if (pid == 0)
            {
                // child code
                // argv = [ name, scheduler-process-id ]
                char scheduler_id[15];
                char scheduler_name[5];
                char remaining_time[15];
                char process_id[15];
                
                sprintf(process_id, "%d", picked_proc.processId);
                sprintf(scheduler_id, "%d", getppid());
                sprintf(scheduler_name, "%d", HPF);
                sprintf(remaining_time, "%d", picked_proc.remainingTime);
                char *argv[] = {"./process.out", remaining_time, scheduler_id, scheduler_name, process_id, 0};
                execve(argv[0], &argv[0], NULL);
            }
            else
            {
                // parent code
                raise(SIGSTOP);
            }
        }
    }

    destroyClk(true);
}

void sig_child_handler(int signum)
{
    int finish_time = getClk();

    char log_message[100];
    sprintf(log_message, "Process %d finished at %d", 1, finish_time);

    write_to_file("proc.txt", log_message);

}
