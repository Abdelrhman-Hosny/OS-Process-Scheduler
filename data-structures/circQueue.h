#include<stdlib.h>
#include<stdio.h>
#include<string.h>

struct Node
{
    int data;
    struct Node* next;
};

struct CircularQueue
{
    struct Node* front;
    struct Node* rear;
};

struct CircularQueue initializeQueue()
{
    struct CircularQueue queue;
    queue.front = NULL;
    queue.rear = NULL;
    return queue;
}

int isEmpty(struct CircularQueue* queue)
{
    return queue->front == NULL;
}

void push(struct CircularQueue* queue, int data)
{
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->data = data;
    temp->next = NULL;

    if(isEmpty(queue))
    {
        queue->front = temp;
        queue->rear = temp;
    }
    else
    {
        queue->rear->next = temp;
        queue->rear = temp;
    }
}

int pop(struct CircularQueue* queue)
{
    if(isEmpty(queue))
    {
        return -1;
    }

    // 
    struct Node* temp = queue->front;
    queue->front = queue->front->next;

    // if empty make front == rear == NULL
    if(queue->front == NULL)
    {
        queue->rear = NULL;
    }

    // temp for data
    int temp_data = temp->data;
    free(temp);

    return temp_data;

}


