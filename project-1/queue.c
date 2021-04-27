/* * * * * * * *  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Description : Module for creating and manipulating queues of integers    *
 * Layout by :  Matt Farrugia <matt.farrugia@unimelb.edu.au>                *
 * Created by :Syed Ahammad Newaz Saif                                      *
 * Email : snewaz@student.unimelb.edu.au                                    *
 * Student ID : 684933                                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * */

/* List of libraries for the assignment */
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
//#include "list.h"	// gives us access to everything defined in list.h
#include "queue.h" // and we'll also need the prototypes defined in queue.h

/* A queue is just a wrapper for a list of its items.
   We will use the back of the list as the entry point, and the front as the
   exit point (to take advantage of O(1) insert and remove operations). */

/* * * * * * * * * * * * *
 * FUNCTION DEFINITIONS  *
 * * * * * * * * * * * * */

/* Creates a new queue and returns a pointer to it. */
Queue *new_queue()
{
    Queue *queue = malloc(sizeof *queue);
    assert(queue);

    queue->items = new_list();

    return queue;
}

/* Destroys a queue and its associated memory. */
void free_queue(Queue *queue)
{
    assert(queue != NULL);
    // free the list of items, and the queue itself
    free_list(queue->items);
    free(queue);
}

/* Inserts a new item at the back of a queue. O(1). */
void queue_enqueue(Queue *queue, Data data)
{
    assert(queue != NULL);
    // use the back of the list as the entry point
    list_add_end(queue->items, data);
}

/* Removes and returns the item at the front of a queue. O(1).
   Error if the queue is empty (so first ensure queue_size() > 0). */
Data queue_dequeue(Queue *queue)
{
    assert(queue != NULL);
    assert(queue_size(queue) > 0);

    // use the front of the list as the exit point
    return list_remove_start(queue->items);
}

/* Returns the number of items currently in a queue. */
int queue_size(Queue *queue)
{
    assert(queue != NULL);
    // delegate straight to list size function
    return list_size(queue->items);
}

/* Bubble sort the given linked list. O(n^2). */
void bubbleSort(Queue *queue)
{
    int swapped;
    Node *ptr1;
    Node *lptr = NULL;

    /* Checking for empty list */
    if (queue == NULL)
        return;

    do
    {
        swapped = 0;
        ptr1 = queue->items->head;

        // the loop keeps on checking for any unordered element and swaps
        // accordingly
        while (ptr1->next != lptr)
        {
            if (ptr1->data.remainingtime > ptr1->next->data.remainingtime)
            {
                // sort by remaining time
                swap(ptr1, ptr1->next);
                swapped = 1;
            }
            else if (ptr1->data.remainingtime == ptr1->next->data.remainingtime && ptr1->data.processid > ptr1->next->data.processid)
            {
                // sort by processids if remaining time are the same
                swap(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

/* Bubble sorts the given linked list by its executiontime in 
   descending order. O(n^2).  */
void bubbleSortbyExecutionTime(Queue *queue)
{
    int swapped;
    Node *ptr1;
    Node *lptr = NULL;

    // Checking for empty list.
    if (queue == NULL)
        return;
    do
    {
        swapped = 0;
        ptr1 = queue->items->head;

        // The loop keeps on checking for any unordered element and swaps
        // accordingly.
        while (ptr1->next != lptr)
        {
            if (ptr1->data.executiontime < ptr1->next->data.executiontime)
            {
                // sort by execution time
                swap(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

/* Function to swap data of two nodes a and b. */
void swap(Node *a, Node *b)
{
    Data temp = a->data;
    a->data = b->data;
    b->data = temp;
}