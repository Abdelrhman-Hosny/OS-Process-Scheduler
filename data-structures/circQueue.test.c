
#include "./circQueue.h"

int main(int argc, char const *argv[])
{
    struct CircularQueue queue = initializeQueue();
    push(&queue, 1);
    push(&queue, 2);
    push(&queue, 3);
    push(&queue, 4);

    int i;
    
    i = pop(&queue);
    printf("%d ", i);

    i = pop(&queue);
    printf("%d ", i);

    push(&queue, 1);
    push(&queue, 2);

    while (!isEmpty(&queue))
    {
        i = pop(&queue);
        printf("%d ", i);
    }

    push(&queue, 1);
    push(&queue, 2);
    push(&queue, 3);
    

    while (!isEmpty(&queue))
    {
        i = pop(&queue);
        printf("%d ", i);
    }

    printf("\n");
    /* code */
    return 0;
}
