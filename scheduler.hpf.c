#include "headers.h"

struct msgbuffer
{
    long mtype;
    struct process proc;
};

void sig_child_handler(int);
void sig_processGen_finish(int);
void sig_processGen_handler(int);
void sig_int_handler(int);

int msgid;
int isFinished_ProcGen = 0, total_processes = 0;
struct MinHeap heap;
struct process running_process;

int processRunning; // 0 if no process currently running
                    // 1 if there is currently a process running

float Ex = 0, Ex2 = 0; // used to calculate std dev of WTA
                       // using the formula: Ex = E[x^2] - E[x]^2
float total_wait_time = 0;
float total_useful_time = 1; // because we start from t = 1;

int main(int argc, char *argv[])
{

    signal(SIGUSR1, sig_processGen_handler);

    signal(SIGUSR2, sig_processGen_finish);

    heap = initializeMinHeap();
    signal(SIGCHLD, sig_child_handler);

    // handling signal sent by process generator made it SIGCONT(and sigcont sent by child will be handled by SIGCHILD)

    signal(SIGINT, sig_int_handler);

    initClk();
    processRunning = 0; // initially no process is running

    // init message queue
    key_t key = ftok("./clk.c", 'a');
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1)
    {
        perror("msgget");
        return 1;
    }

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

        initiate_process(&picked_proc, getClk() - picked_proc.arrivalTime, &total_wait_time);
        running_process = copyProcess(picked_proc);

        // start here

        char log_message[100];
        sprintf(log_message, "At time %d process %d started arr %d total %d remain %d wait %d", running_process.startTime, running_process.processId, running_process.arrivalTime, running_process.runTime, running_process.remainingTime, running_process.waitingTime);

        write_to_file(log_file, log_message);
        // end here
        pop_heap(&heap);
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
    print_statistics(Ex, Ex2, total_processes, total_wait_time, total_useful_time);
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
        running_process.state = TERMINATED;
        running_process.finishTime = getClk();

        total_useful_time += running_process.runTime;
        int TA = running_process.finishTime - running_process.arrivalTime;
        float WTA = (float)TA / running_process.runTime;
        Ex += WTA;
        Ex2 += WTA * WTA;

        char log_message[100];
        sprintf(log_message, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f", running_process.finishTime, running_process.processId, running_process.arrivalTime, running_process.runTime, running_process.remainingTime, running_process.waitingTime, TA, WTA);
        write_to_file(log_file, log_message);

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
            total_processes++;
            struct process proc = msg.proc;
            push_heap(&heap, proc, proc.priority);
            // char log_message[100];
            // sprintf(log_message, "Process %d added to heap at time %d , top id = %d", proc.processId, getClk(), processRunning);
            // write_to_file("proc.txt", log_message);
        }
    }

    if (processRunning == 0)
    {
        schedule_process();
        // here no need to check if scheduling failed due to empty queue since i just inserted now
        processRunning = 1;
    }
}