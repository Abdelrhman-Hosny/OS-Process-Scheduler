#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>

void my_handler(int sig)
{
    alarm(2);
    printf("Stoped by signal %d\n",sig);
    raise(SIGSTOP);
}
int main(int argc, char const *argv[])
{
    signal(SIGALRM, my_handler);
    alarm(1);
    while(1);
    return 0;
}
