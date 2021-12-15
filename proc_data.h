// struct of process

struct process
{
    int runTime;
    int priority;
    int processId;
    int arrivalTime;

    int state;
    int waitingTime;
    int executionTime;
    int remainingTime;
    int finishTime;
};

struct process initializeProcess(
    int runtime, int priority,
    int processId, int arrivalTime)
{
    struct process p;
    p.runTime = runtime;
    p.priority = priority;
    p.processId = processId;
    p.arrivalTime = arrivalTime;
    p.remainingTime = runtime;

    p.state = 0;
    p.waitingTime = 0;
    p.executionTime = 0;
    p.finishTime = -1;
    return p;
}

void initializeProcessPointer(
    struct process *p,
    int runtime, int priority,
    int processId, int arrivalTime)
{
    p->runTime = runtime;
    p->priority = priority;
    p->processId = processId;
    p->arrivalTime = arrivalTime;
    p->remainingTime = runtime;

    p->state = 0;
    p->waitingTime = 0;
    p->executionTime = 0;
    p->finishTime = -1;
}

struct process copyProcess(struct process input)
{
    struct process copied;
    copied.runTime = input.runTime;
    copied.priority = input.priority;
    copied.processId = input.processId;
    copied.arrivalTime = input.arrivalTime;
    copied.state = input.state;
    copied.waitingTime = input.waitingTime;
    copied.executionTime = input.executionTime;
    copied.remainingTime = input.remainingTime;
    copied.finishTime = input.finishTime;
    return copied ;
};