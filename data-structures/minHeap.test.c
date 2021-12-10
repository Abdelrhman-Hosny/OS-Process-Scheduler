#include "./minHeap.h"
#include<stdio.h>

int main(int argc, char const *argv[])
{
    struct MinHeap minHeap =  initializeMinHeap();

    push(&minHeap, 2, 2);
    printf("%d \n", peek(& minHeap));

    push(&minHeap, 1, 1);

    printf("%d \n", peek(& minHeap));
    pop(&minHeap);
    printf("%d \n", peek(& minHeap));

    push(&minHeap, 2, 0);
    printf("%d \n", peek(& minHeap));
    

    return 0;
}
