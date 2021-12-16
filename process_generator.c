#include "headers.h"
#include "readingFiles.h"

struct msgbuffer
{
    long mtype;
    struct process proc;
};
void alarm_handler(int);
int msgid, currentProcessIndex=0, timeToWait;
void clearResources(int);
void handle_scheduler_sigchild(int);
struct process *processes;
int processCount;
int scheduler_fork;
void initial_send_to_scheduler();

int main(int argc, char *argv[])
{
    signal(SIGALRM, alarm_handler);
    signal(SIGINT, clearResources);
    signal(SIGCHLD, handle_scheduler_sigchild);

    int clock_fork = fork();
    if (clock_fork == 0)
    {
        // clk code
        //clk.out
        char *argv[] = {"clk.out", 0};
        execve(argv[0], &argv[0], NULL);
    }

    scheduler_fork = fork();
    if (scheduler_fork == 0)
    {
        // scheduler code
        //scheduler.out
        char *argv[] = {"scheduler.rr.out", 0};
        execve(argv[0], &argv[0], NULL);
    }


    char *fileName = "processes.txt";
    processCount = 0;
    processes = readProccesses(fileName, &processCount);
    for (int i = 0; i < processCount; i++)
    {
        printf("%d %d %d %d\n", processes[i].processId, processes[i].arrivalTime, processes[i].remainingTime, processes[i].priority);
    }

    initClk();
    
    int x = getClk();
    // printf("current time is %d\n", x);
    
    key_t key = ftok("./clk.c", 'a');
    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1)
    {
        perror("msgget");
        return 1;
    }

    

    initial_send_to_scheduler();

    timeToWait = processes[currentProcessIndex].arrivalTime - getClk();

    alarm(timeToWait);
    while (1)
    {
        pause();
    }
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    destroyClk(true);
    // clear msg queue
    msgctl(msgid, IPC_RMID, (struct msqid_ds *)0);
    // scheduler
    exit(0);
}

void handle_scheduler_sigchild(int signum)
{

    int pid, status;
    printf("scheduler is dead");

    pid = wait(&status);
    if (WIFEXITED(status))
    {
        if (pid == scheduler_fork)
            raise(SIGINT);
    }
}

void initial_send_to_scheduler()
{
    if (processes[currentProcessIndex].arrivalTime == 0)
    {
        // printf("new function\n");
        struct msgbuffer msg;
        msg.mtype = 1;
        msg.proc = processes[currentProcessIndex];
        if (msgsnd(msgid, &msg, sizeof(msg.proc), 0) == -1)
        {
            perror("msgsnd");
            exit(1);
        }
        // printf("sent process %d\n", currentProcessIndex);
        // 2. Increment the current process index.
        currentProcessIndex++;

        while (currentProcessIndex < processCount &&
               processes[currentProcessIndex].arrivalTime == processes[currentProcessIndex - 1].arrivalTime)
        {

            struct msgbuffer msg;
            msg.mtype = 1;
            msg.proc = processes[currentProcessIndex];
            if (msgsnd(msgid, &msg, sizeof(msg.proc), 0) == -1)
            {
                perror("msgsnd");
                exit(1);
            }
            // printf("sent process %d\n", currentProcessIndex);
            // 2. Increment the current process index.
            currentProcessIndex++;
        }

        kill(scheduler_fork, SIGUSR1); // sending signal to scheduler to recieve process from buffer

        // TODO Exit the program when processes are finished not sent.
        if (currentProcessIndex == processCount)
        {
            printf("finished\n");
            kill(scheduler_fork, SIGUSR2);
        }
    }
}

void alarm_handler(int sig)
{
    //TODO Handle the alarm signal
    // 1. Send the process to the scheduler.
    // printf("alarm\n");

    struct msgbuffer msg;
    msg.mtype = 1;
    msg.proc = processes[currentProcessIndex];
    if (msgsnd(msgid, &msg, sizeof(msg.proc), 0) == -1)
    {
        perror("msgsnd");
        exit(1);
    }
    // printf("sent process %d\n", currentProcessIndex);
    // 2. Increment the current process index.
    currentProcessIndex++;

    while (currentProcessIndex < processCount &&
           processes[currentProcessIndex].arrivalTime == processes[currentProcessIndex - 1].arrivalTime)
    {

        struct msgbuffer msg;
        msg.mtype = 1;
        msg.proc = processes[currentProcessIndex];
        if (msgsnd(msgid, &msg, sizeof(msg.proc), 0) == -1)
        {
            perror("msgsnd");
            exit(1);
        }
        // printf("sent process %d\n", currentProcessIndex);
        // 2. Increment the current process index.
        currentProcessIndex++;
    }

    kill(scheduler_fork, SIGUSR1); // sending signal to scheduler to recieve process from buffer

    // 3. Calculate the time to wait for the next process.
    timeToWait = processes[currentProcessIndex].arrivalTime - getClk();

    // TODO Exit the program when processes are finished not sent.
    if (currentProcessIndex == processCount)
    {
        printf("finished\n");
        kill(scheduler_fork, SIGUSR2);
    }
    else
    {
        // 4. Restart the alarm.
        alarm(timeToWait);
        // printf("time to wait is %d\n", timeToWait);
    }

    signal(SIGALRM, alarm_handler);
}
