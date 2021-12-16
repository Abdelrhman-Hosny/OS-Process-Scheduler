#include "headers.h"

struct msgbuffer
{
    long mtype;
    struct process proc;
};
int start_time;
void sig_child_handler(int);
void sig_processGen_finish(int);
void sig_processGen_handler(int);
void sig_int_handler(int);

int quantum = 1;
int msgid;
int isFinished_ProcGen = 0;
struct CircularQueue myQueue;

struct process currentlyProcessing; // will hold processes currently processing

int processRunning; // 0 if no process currently running
                    // 1 if there is currently a process running

int main(int argc, char *argv[])
{

    signal(SIGUSR1, sig_processGen_handler);
    signal(SIGUSR2, sig_processGen_finish);
    myQueue = initializeQueue();
    key_t key = ftok("./clk.c", 'a');
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1)
    {
        char *error = "msgget";
        write_to_file("proc.txt", error);
        perror("msgget");
        return 1;
    }

    initClk();

    signal(SIGCHLD, sig_child_handler);

    // handling signal sent by process generator made it SIGCONT(and sigcont sent by child will be handled by SIGCHILD)

    signal(SIGINT, sig_int_handler);

    processRunning = 0; // initially no process is running

    // init message queue

    while (1)
    {
        // parent code
        pause();
    }
}

// return 1 if scheduling process succeeded; 0 if no process in queue so failed
int schedule_process()
{
    struct process picked_proc = peek_queue(&myQueue);

    if (picked_proc.arrivalTime != -1)
    {
        // Queue is not empty
        // After the schedule picks the process
        // it will be executed

        // editing before entering fork !!
        int timeForProcess = (quantum < picked_proc.remainingTime) ? quantum : picked_proc.remainingTime;
        currentlyProcessing = copyProcess(picked_proc);
        pop_queue(&myQueue);
        currentlyProcessing.remainingTime -= timeForProcess;

        int pid = fork();

        if (pid == -1)
            perror("error in fork");

        else if (pid == 0)
        {
            // child code
            // add parameters to child process
            char scheduler_id[15];
            char scheduler_name[5];
            char remaining_time[15];
            char process_id[15];
            sprintf(process_id, "%d", picked_proc.processId);
            sprintf(scheduler_id, "%d", getppid());
            sprintf(scheduler_name, "%d", RR);
            sprintf(remaining_time, "%d", timeForProcess);
            // decreasing remaining time of process which will enter by either quantum or (its life time if smaller)
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

    int pid, status;
    pid = wait(&status);
    if (WIFEXITED(status))
    {

        processRunning = 0; // meaning no process is running now
        int finish_time = getClk();

        if (WEXITSTATUS(status) == currentlyProcessing.processId && currentlyProcessing.remainingTime != 0)
        {
            char log_message[100];
            sprintf(log_message, "Process %d paused at %d , remaining time %d", WEXITSTATUS(status), finish_time, currentlyProcessing.remainingTime);
            write_to_file("proc.txt", log_message);
            push_queue(&myQueue, currentlyProcessing);
        }
        else
        {
            // discard process
            char log_message[100];
            sprintf(log_message, "Process %d finished at %d", WEXITSTATUS(status), finish_time);
            write_to_file("proc.txt", log_message);
        }

        if (peek_queue(&myQueue).arrivalTime == -1 && // queue is empty
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
            // add process to queue
            struct process proc = msg.proc;
            char log_message[100];
            push_queue(&myQueue, proc);
            sprintf(log_message, "Process %d added to queue at time %d , top id = %d", proc.processId, getClk(), processRunning);
            write_to_file("proc.txt", log_message);
        }
    }

    if (processRunning == 0)
    {
        schedule_process();
        // here no need to check if scheduling failed due to empty queue since i just inserted now
        processRunning = 1;
    }
}