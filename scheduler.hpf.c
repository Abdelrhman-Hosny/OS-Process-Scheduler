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
            //upon termination release the clock resources

            // After the schedule picks the process
            // it will be executed

            start_time = getClk();

            int pid = fork();

            if (pid == -1)
                perror("error in fork");

            else if (pid == 0)
            {
                // child code
                printf("arr time : %d\n", picked_proc.arrivalTime);
                // argv = [ name, scheduler-process-id ]
                char *argv[] = {"./process.out",picked_proc.remainingTime, getppid(),HPF, 0};
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

    write_to_file("output.txt", "At time, Process is terminated\n");

    raise(SIGCONT);
}
