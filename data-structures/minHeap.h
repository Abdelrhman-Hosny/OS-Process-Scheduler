#include<stdlib.h>
#include<stdio.h>
#include<string.h>

struct Node
{
    int data;
 
    // Lower values indicate
    // higher priority
    int priority;
 
    struct Node* next;
 
};

struct MinHeap
{
    struct Node* head;
    
};

struct MinHeap initializeMinHeap()
{
    struct MinHeap minHeap;
    minHeap.head = NULL;
    return minHeap;
}

// Function to create a new node
struct Node* newNode(int d, int p)
{
    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->data = d;
    temp->priority = p;
    temp->next = NULL;
 
    return temp;
}
 
// Return the value at head
int peek(struct MinHeap * heap)
{
    if (heap->head == NULL)
        return -1;
    else
    return heap->head->data;
}
 
// Removes the element with the
// highest priority form the list
int pop(struct MinHeap *heap)
{
    struct Node* temp = heap->head;
    heap->head = heap->head->next;
    int data = temp->data;
    free(temp);

    return data;
}
 
// Function to push according to priority
void push(struct MinHeap* heap, int d, int p)
{
    struct Node * head = heap->head;
    struct Node* start = head;
    if (head==NULL)
    {
        heap->head = newNode(d, p);
        return;
    }
    
    // Create new Node
    struct Node* temp = newNode(d, p);
 
    // Special Case: The head of list has
    // lesser priority than new node. So
    // insert newnode before head node
    // and change head node.
    if (head->priority > p)
    {
         printf("dashda %d \n", d);
        // Insert New Node before head
        temp->next = head;
        head = temp;
        heap->head = head;
    }
    else
    {
         
        // Traverse the list and find a
        // position to insert new node
        while (start->next != NULL &&
            start->next->priority < p)
        {
            start = start->next;
        }
 
        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}
 
// Function to check is list is empty
int isEmpty(struct MinHeap* heap)
{
    return (heap->head) == NULL;
}