/* * * * * * * *  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Description : Module for creating and manipulating queues of integers    *
 * Layout by :  Matt Farrugia <matt.farrugia@unimelb.edu.au>                *
 * Created by :Syed Ahammad Newaz Saif                                      *
 * Email : snewaz@student.unimelb.edu.au                                    *
 * Student ID : 684933                                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * */

/* 'include guards': These lines (and #endif at the bottom) prevent this file
    from being included multiple times. multiple inclusion can cause errors on
    some compilers, especially if the included file contains struct definitions
    how it works: https://en.wikipedia.org/wiki/Include_guard */

#ifndef QUEUE_H
#define QUEUE_H

#include "list.h"

/* * * * * * * * * * *
 * TYPE DEFINITIONS  *
 * * * * * * * * * * */

typedef struct queue Queue;

/* * * * * * * * * *
 * DATA STRUCTURES *
 * * * * * * * * * */

struct queue
{
    List *items;
};

/* * * * * * * * * * * * *
 * FUNCTION DEFINITIONS  *
 * * * * * * * * * * * * */

/* Creates a new queue and returns a pointer to it. */
Queue *new_queue();

/* Destroys a queue and its associated memory. */
void free_queue(Queue *queue);

/* Inserts a new item at the back of a queue. O(1). */
void queue_enqueue(Queue *queue, Data data);

/* Removes and returns the item at the front of a queue. O(1).
   Error if the queue is empty (so first ensure queue_size() > 0). */
Data queue_dequeue(Queue *queue);

/* Returns the number of items currently in a queue. */
int queue_size(Queue *queue);

/* Bubble sort the given linked list. O(n^2). */
void bubbleSort(Queue *queue);

/* Bubble sorts the given linked list by its executiontime in 
   descending order. O(n^2).  */
void bubbleSortbyExecutionTime(Queue *queue);

/* Function to swap data of two nodes a and b. */
void swap(Node *a, Node *b);

#endif
