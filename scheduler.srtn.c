#include "headers.h"

struct process copyProcess(struct process proc)
{
    return proc;
}

struct msgbuffer
{
    long mtype;
    struct process proc;
};
int start_time;
int glob_pid;
void sig_child_handler(int);
void sig_processGen_finish(int);
void sig_processGen_handler(int);
void sig_int_handler(int);

int msgid;
int isFinished_ProcGen = 0;
struct MinHeap heap;
int last_run_time = 0;

struct process running_process;
int processRunning; // 0 if no process currently running
                    // 1 if there is currently a process running

int main(int argc, char *argv[])
{

    initClk();

    signal(SIGCHLD, sig_child_handler);

    // handling signal sent by process generator made it SIGCONT(and sigcont sent by child will be handled by SIGCHILD)
    signal(SIGUSR1, sig_processGen_handler);

    signal(SIGUSR2, sig_processGen_finish);

    signal(SIGINT, sig_int_handler);

    processRunning = 0; // initially no process is running

    // init message queue
    key_t key = ftok("./clk.c", 'a');
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1)
    {
        perror("msgget");
        return 1;
    }

    heap = initializeMinHeap();

    while (1)
    {
        // parent code
        pause();
    }
}

// return 1 if scheduling process succeeded; 0 if no process in queue so failed
int schedule_process()
{
    struct process picked_proc = peek_heap(&heap);

    if (picked_proc.arrivalTime != -1)
    {

        // Queue is not empty
        // After the schedule picks the process
        // it will be executed

        running_process = copyProcess(picked_proc);
        pop_heap(&heap);
        last_run_time = getClk();

        glob_pid = fork();

        if (glob_pid == -1)
            perror("error in fork");

        else if (glob_pid == 0)
        {
            // child code

            // add parameters to child process
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

        return 1;
    }
    return 0;
}

void sig_int_handler(int signum)
{
    // scheduler cleanup
    destroyClk(false);

    exit(0);
}

void sig_child_handler(int signum)
{
    char log_message[100];
    sprintf(log_message, "recieved sigchild at %d", getClk());
    write_to_file("proc.txt", log_message);

    int pid, status;

    pid = wait(&status);
    if (WIFEXITED(status))
    {

        if (WEXITSTATUS(status) != 0)
        {
            processRunning = 0; // meaning no process is running now
            int finish_time = getClk();

            char log_message[100];
            sprintf(log_message, "Process %d finished at %d", WEXITSTATUS(status), finish_time);
            write_to_file("proc.txt", log_message);

            if (peek_heap(&heap).arrivalTime == -1 && // heap is empty
                isFinished_ProcGen == 1)
            {
                raise(SIGINT);
            }
            int succeeded = schedule_process();
            if (succeeded)
            {
                processRunning = 1;
            }
        }
    }
}

void sig_processGen_finish(int signum)
{
    isFinished_ProcGen = 1;
}

void sig_processGen_handler(int signum)
{

    // recieving processes will be here in handler
    int rec_val = 0;
    while (rec_val != -1) // meaning message queue is empty
    {
        struct msgbuffer msg;
        rec_val = msgrcv(msgid, (struct msgbuffer *)&msg, sizeof(msg.proc), 0, IPC_NOWAIT);

        if (rec_val != -1) // there's something to recover from queue
        {
            // add process to heap
            struct process proc = msg.proc;
            char log_message[100];
            sprintf(log_message, "Process %d added to heap at time %d , Proc Running State = %d", proc.processId, getClk(), processRunning);
            write_to_file("proc.txt", log_message);
            push_heap(&heap, proc, proc.remainingTime);
        }
    }

    // interrupt running process and update it
    if (processRunning == 1)
    {
        running_process.remainingTime -= (getClk() - last_run_time);
        last_run_time = getClk();
        struct process next_proc = peek_heap(&heap);
        if (next_proc.remainingTime < running_process.remainingTime)
        {
            // kill old process and return to heap
            char log_message[100];
            sprintf(log_message, "Process %d replaced at time %d, remaining time : %d", running_process.processId, getClk(), running_process.remainingTime);
            write_to_file("proc.txt", log_message);

            kill(glob_pid, SIGUSR1);
            processRunning = 0;
            push_heap(&heap, running_process, running_process.remainingTime);
        }
    }

    if (processRunning == 0)
    {
        schedule_process();
        // here no need to check if scheduling failed due to empty queue since i just inserted now
        processRunning = 1;
    }
}