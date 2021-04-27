/* * * * * * * *  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Description : Module for creating and manipulating singly-linked lists   *
 *               of integers                                                *
 * Layout by :  Matt Farrugia <matt.farrugia@unimelb.edu.au>                *
 * Created by :Syed Ahammad Newaz Saif                                      *
 * Email : snewaz@student.unimelb.edu.au                                    *
 * Student ID : 684933                                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * */

#ifndef _LIST_H
#define _LIST_H
#include <stdbool.h>

typedef struct list List;
/* Helper function : Creates a new list and returns a pointer to it. */
List *new_list();

/* * * * * * * * * * *
 * TYPE DEFINITIONS  *
 * * * * * * * * * * */
 
typedef struct node Node;
typedef struct data Data;
typedef struct compresseddata CompressedData;

/* * * * * * * * * *
 * DATA STRUCTURES *
 * * * * * * * * * */

/* A list of relevant fields of data */
struct data
{
    int arrivaltime;
    double processid;
    int executiontime;
    char parallelisable;
    int completiontime;
    int turnaroundtime;
    int remainingtime;
    double timeoverhead;
    int totalremainingtime;
    // embedding the parent information with a process
    int parentid;
    int parentexecutiontime;
    int parentarrivaltime;
    int parentcompletiontime;
    int parentturnaroundtime;
    double parenttimeoverhead;
};

/* A data structure handy when keeping track of subprocesses */
struct compresseddata
{
    int parentid;
    int number_child;
    int subprocess; //flag to check subprocess
};

/* A list node points to the next node in the list, 
   and to some data */
struct node
{
    Node *next;
    Data data;
};

/* A list that points to its first and last nodes, and 
   stores its size (num.nodes). */
struct list
{
    Node *head;
    Node *tail;
    int size;
};

/* * * * * * * * * * * * *
 * FUNCTION DEFINITIONS  *
 * * * * * * * * * * * * */

/* Destroys a list and frees its memory. */
void free_list(List *list);

/* Adds an element to the front of a list.
   This operation is O(1). */
void list_add_start(List *list, Data data);

/* Adds an element to the back of a list.
   This operation is O(1). */
void list_add_end(List *list, Data data);

/* Removes and returns the front data element from a list.
   This operation is O(1).
   Error if the list is empty (so first ensure list_size() > 0). */
Data list_remove_start(List *list);

/* Removes and returns the final data element in a list.
   This operation is O(n), where n is the number of elements in the list.
   Error if the list is empty (so first ensure list_size() > 0 ). */
Data list_remove_end(List *list);

/* Returns the number of elements contained in a list. */
int list_size(List *list);

/* Returns whether the list contains no elements (true) or some elements (false). */
bool list_is_empty(List *list);

#endif
