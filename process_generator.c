#include "headers.h"
#include "readingFiles.h"

struct msgbuffer {
    long mtype;
    struct process proc;
};
void alarm_handler(int sig); 
int msgid, currentProcessIndex, timeToWait;
void clearResources(int);
struct process* processes ;
int processCount ;

int main(int argc, char * argv[])
{
    signal(SIGALRM, alarm_handler);
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    char* fileName = "processes.txt";
    processCount = 0;
    processes = readProccesses(fileName,&processCount);
    // for (int i = 0; i < processCount; i++)
    // {
    //     printf("%d %d %d %d\n", processes[i].processId, processes[i].arrivalTime, processes[i].remainingTime, processes[i].priority);
    // }
    
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    // 3. Initiate and create the scheduler and clock processes.
    int clock_fork = fork();
    if(clock_fork ==0 )
    {
        // clk code
        //clk.out
        char * argv[] = {"clk.out", 0};
        execve(argv[0], &argv[0], NULL);
    }


    int scheduler_fork = fork();
    if(scheduler_fork == 0)
    {
        // scheduler code
        //scheduler.out
        char * argv[] = {"scheduler.out", 0};
        execve(argv[0], &argv[0], NULL);
    }


    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 6. Send the information to the scheduler at the appropriate time.
    key_t key = ftok("./clk.c", 'a');
    msgid = msgget(key, IPC_CREAT | 0666);
    if(msgid == -1){
        perror("msgget");
        return 1;
    }

    currentProcessIndex = 0 ;
    timeToWait = processes[currentProcessIndex].arrivalTime - getClk();

    alarm(timeToWait);
    while(1);
    
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    destroyClk(true);
    // clear msg queue
    msgctl(msgid, IPC_RMID, (struct msqid_ds *) 0);
    raise(SIGKILL);

}

void alarm_handler(int sig) {
    //TODO Handle the alarm signal
    // 1. Send the process to the scheduler.
    printf("alarm\n");
    struct msgbuffer msg;
    msg.mtype = 1;
    msg.proc = processes[currentProcessIndex];
    if(msgsnd(msgid, &msg, sizeof(msg.proc), 0) == -1)
    {
        perror("msgsnd");
        exit(1);
    }
    printf("sent process %d\n", currentProcessIndex);
    // 2. Increment the current process index.
    currentProcessIndex++;
    // 3. Calculate the time to wait for the next process.
    timeToWait = processes[currentProcessIndex].arrivalTime - getClk();
    
    if(currentProcessIndex==processCount)
    {   
        printf("finished\n");
        raise(SIGINT);
    }
    // 4. Restart the alarm.
    alarm(timeToWait);
    printf("time to wait is %d\n", timeToWait);
    pause();
    
}
