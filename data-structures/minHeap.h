#include<stdlib.h>
#include<stdio.h>
#include<string.h>

struct MinHeapNode
{
    struct process data;
 
    // Lower values indicate
    // higher priority
    int priority;
 
    struct MinHeapNode* next;
 
};

struct MinHeap
{
    struct MinHeapNode* head;
    
};

struct MinHeap initializeMinHeap()
{
    struct MinHeap minHeap;
    minHeap.head = NULL;
    return minHeap;
}

// Function to create a new MinHeapNode
struct MinHeapNode* newMinHeapNode(struct process d, int p)
{
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    temp->data = d;
    temp->priority = p;
    temp->next = NULL;
 
    return temp;
}
 
// Return the value at head
struct process peek_heap(struct MinHeap * heap)
{
    if (heap->head == NULL)
    {
        struct process data = initializeProcess(0,0,0, -1, 5);
        return data;
    }
    else
    return heap->head->data;
}
 
// Removes the element with the
// highest priority form the list
void pop_heap(struct MinHeap *heap)
{
    if (heap->head == NULL)
        return;
    struct MinHeapNode* temp = heap->head;
    heap->head = heap->head->next;
    free(temp);
}
 
// Function to push according to priority
void push_heap(struct MinHeap* heap, struct process d, int p)
{
    struct MinHeapNode * head = heap->head;
    struct MinHeapNode* start = head;
    if (head==NULL)
    {
        heap->head = newMinHeapNode(d, p);
        return;
    }
    
    // Create new MinHeapNode
    struct MinHeapNode* temp = newMinHeapNode(d, p);
 
    // Special Case: The head of list has
    // lesser priority than new MinHeapNode. So
    // insert newMinHeapNode before head MinHeapNode
    // and change head MinHeapNode.
    if (head->priority > p)
    {
        // Insert New MinHeapNode before head
        temp->next = head;
        head = temp;
        heap->head = head;
    }
    else
    {
         
        // Traverse the list and find a
        // position to insert new MinHeapNode
        while (start->next != NULL &&
            start->next->priority <= p)
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
int isEmpty_heap(struct MinHeap* heap)
{
    return (heap->head) == NULL;
}