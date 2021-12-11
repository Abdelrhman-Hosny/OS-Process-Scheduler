#include "headers.h"

int main(int argc, char *argv[])
{
    initClk();
    initShmProc();

    while (1)
    {

        // shm = p1

        //TODO implement the scheduler :)
        //upon termination release the clock resources

        // After the schedule picks the process
        // it will be executed
        struct process picked_proc = initializeProcess(10, 1, 1, 0);

        // add proc to shared memory
        setShmProc(&picked_proc);

        int pid = fork();

        if (pid == -1)
            perror("error in fork");

        else if (pid == 0)
        {
            // child code

            // argv = [ name, scheduler-process-id ]
            char *argv[] = {"./process", stoi(getppid()), 0};
            execve(argv[0], &argv[0], NULL);
        }
        else
        {
            // parent code

            raise(SIGSTOP);

            // p1 = *shm
        }
    }

    destroyClk(true);
    destroyShmProc(true);
}
