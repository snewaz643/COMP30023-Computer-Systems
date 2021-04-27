/* * * * * * * *  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Description : Main driver program for process scheduling that works      *
 *               with both parallelisable and non-parallelisable processes  *
 *               for both single or multiprocessor using the shortest       *
 *               remaining time algorithm                                   *
 * Resources :  https://www.youtube.com/watch?v=_QcX99B-zbU&t=707s          *
 * Created by :Syed Ahammad Newaz Saif                                      *
 * Email : snewaz@student.unimelb.edu.au                                    *
 * Student ID : 684933                                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * */

/* List of libraries allowed for this project from the given specification */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <limits.h>

/* Accompanying libraries created to facilitate the simulation */
#include "queue.h"
#include "list.h"

/* Global variables that helps structure my code */
#define LINEMAX 1024
#define MINTOTALSTDIN 4
#define MAXTOTALSTDIN 7
#define FILEFLAG "-f"
#define NPROCESSOR "-p"
#define ELEMENTS 5
#define SINGLE_PROCESSOR 1
#define CHALLENGE "-c"

/* Reads from stdin in the form :
   './allocate (executeable main) -f testfilepath -p n (number of processes)'.
   An optional  '-c' would then work and manipulate the scheduler to reduce the makespan.
   The -f and -p flag can come in any order. Stores the nprocessor and the filepath.
   Returns filepath. */
char *readfromstdin(int argc, char *argv[], int *nprocessor, char *filepath, int *challenge)
{
    int argument = 0;

    // read from stdin
    if (argc > MINTOTALSTDIN && argc < MAXTOTALSTDIN)
    {
        for (argument = 1; argument < argc; argument++)
        {
            // check '-f' flag (any order)
            if (strcmp(argv[argument], FILEFLAG) == 0)
            {
                // store the filepath
                filepath = argv[argument + 1];
            }
            else if (strcmp(argv[argument], NPROCESSOR) == 0)
            {
                // check for '-p' flag (any order), convert to integer
                (*nprocessor) = atoi(argv[argument + 1]);
            }
            else if (strcmp(argv[argument], CHALLENGE) == 0)
            {
                // if the own scheduling algorithm prompted by '-c' works
                (*challenge) = 1;
            }
        }
    }
    else
    {
        // incorrect reading from stdin exception
        fprintf(stderr, "Not enough arguments are present : Expected atleast 4 (inclusive of challenge flag)!");
    }
    return filepath;
}

/* Reads the filepath onto a filereader, opening and reading line by line.
   Stores all information onto a read-in queue and initializes the other
   relevant fields and takes note of total execution time and count.
   Returns the read-in queue. */
Queue *readfile(char *filepath, Queue *readinqueue, int *totalexecutiontime, int *count, int challenge)
{
    FILE *file1;
    char inputfile[LINEMAX] = "";
    Data data;

    file1 = fopen(filepath, "r"); // filepath used to read the file

    //store the values in a readyqueue
    //readinqueue = new_queue();

    while (fgets(inputfile, LINEMAX, file1))
    {
        // read arrivaltime, processid, executiontime, parallelisability
        sscanf(inputfile, "%d %lf %4d %c\n", &data.arrivaltime, &data.processid, &data.executiontime, &data.parallelisable);
        // intialize the rest of the fields
        data.remainingtime = data.executiontime;
        data.completiontime = 0; // changed this now!
        data.timeoverhead = 0;
        data.turnaroundtime = 0.00;
        data.totalremainingtime = 0;
        data.parentid = -1;             //data.processid;
        data.parentexecutiontime = -1;  //= data.executiontime;
        data.parentarrivaltime = -1;    // = data.parentarrivaltime;
        data.parentcompletiontime = -1; // = 0;
        data.parentturnaroundtime = -1; //= 0.00;
        data.parenttimeoverhead = -1;   //= 0;
        // store the values into the corresponding queue
        if(!readinqueue)
        {
          readinqueue = new_queue();
        }
        queue_enqueue(readinqueue, data);
        // record the totalexecutiontime and count needed later
        (*totalexecutiontime) += data.executiontime;
        (*count)++;
    }
    fclose(file1);

    if (challenge == 1)
    {
        // loop and store the arrival times
        int tmparrivaltimes[*count];

        int i = 0;
        Node *node = readinqueue->items->head;
        while ((i < *count) && node)
        {
            tmparrivaltimes[i] = node->data.arrivaltime;
            i++;
            node = node->next;
        }
        free(node);

        // sort in descending order of execution time
        bubbleSortbyExecutionTime(readinqueue);

        // re enter the arrival times back
        // the aim is to make the heavy processes finish first
        int a = 0;
        Node *newnode = readinqueue->items->head;
        while ((a < *count) && newnode)
        {
            newnode->data.arrivaltime = tmparrivaltimes[a];
            a++;
            newnode = newnode->next;
        }
        free(newnode);
    }

    return readinqueue;
}

/* Finds the sum of the unique processes and subprocesses. */
int find_unique_process_subprocess(int unique[], int index, int curr_queue_size)
{
    int counts = 0;
    int freq[index];
    // set the frequency array to -1
    for (int i = 0; i < index; i++)
    {
        freq[i] = -1;
    }

    // find frequency of each element 
    for (int j = 0; j < index; j++)
    {
        counts = 1;
        for (int k = j + 1; k < index; k++)
        {
            if (unique[j] == unique[k])
            {
                counts++;
                freq[k] = 0;
            }
        }
        if (freq[j] != 0)
        {
            freq[j] = counts;
        }
    }
    
    // find the number of frequent elements
    // eg [pid1 pid1 pid2 pid ] = [2 2] = 2 unique processes
    for (int l = 0; l < index; l++)
    {
        if (freq[l] > 0)
        {
            curr_queue_size += 1;
        }
    }
    return curr_queue_size;
}

/* A handy function to help find total processes.*/
int totalprocess(Queue *readyqueue[], int nprocessor)
{
    int i = 0;
    int totalprocesses = 0;
    // loop through readyqueues
    for (i = 0; i < nprocessor; i++)
    {
        // collect sums of the processes
        Node *node = readyqueue[i]->items->head;
        while (node)
        {
            totalprocesses += 1;
            node = node->next;
        }
    }
    return totalprocesses;
}

/* Takes in a double value.
   Returns the number rounded up to 2 decimal places. */
double roundupdouble(double maxturnaroundtime)
{
    if ((((int)(maxturnaroundtime * 1000)) % 10) >= 5)
    {
        maxturnaroundtime *= 1000;
        maxturnaroundtime += 1;
        maxturnaroundtime /= 1000;
    }
    return maxturnaroundtime;
}

/* Fills up the arrival, executiontime, completion, turnaround and timeoverheads. */
void store_stats(int arrivaltimes[], int executiontimes[], int completiontimes[], int turnaroundtimes[], double timeoverheads[], int index, int arrivaltime, int executiontime, int time)
{
    arrivaltimes[index] = arrivaltime;
    executiontimes[index] = executiontime;
    completiontimes[index] = time;
    turnaroundtimes[index] = completiontimes[index] - arrivaltimes[index];
    timeoverheads[index] = (double)turnaroundtimes[index] / executiontimes[index];
}

/* Marks of fields of process done to signify a subprocess has finished. */
void process_done_processes_parallel(CompressedData*process_done, int index, int parent_id)
{
    // mark for the first time
    if (process_done[index].number_child == -1)
    {
        process_done[index].number_child = 1;
    }
    else
    {
        // mark of and add up to a previous subprocess finished
        process_done[index].number_child += 1;
    }
    process_done[index].parentid = parent_id; // parent id stored
    process_done[index].subprocess = 1; // flag to indicate a subprocess exist
}

/* For a given index corresponding readyqueue corresponding to a cpu a generic function to add values */
void process_done_processes_non_parallel(CompressedData*process_done, int index, int value)
{
    process_done[index].number_child = value;
    process_done[index].parentid = value;
    process_done[index].subprocess = value;
}

/* Initialise the process ids to default values. */
void initialise_process_done(CompressedData *process_done, int maxpid)
{
    // setting the default fields to -1
    // maxpid refers to the maximum value of process id (pid) 
    // found while reading from stdin
    for (int i = 0; i < maxpid; i++)
    {
        process_done[i].parentid = -1;
        process_done[i].number_child = -1;
        process_done[i].subprocess = -1;
    }
}

/*  Uses the count as number of processes,their turnaroundtimes,
    timeoverheads and total execution time to print out maximum,
    average timeoverheads, turnaroundtime, makespan for the simulation */
void printoutstats(int count, int nprocessor, int turnaroundtime[], double timeoverhead[], int time)
{
    int i = 0;
    int j = 0;
    double averageturnaroundtime = 0.00;
    double averagetimeoverhead = 0.00;
    double maxturnaroundtime = 0.00;

    for (i = 0; i < count; i++)
    {
        if (turnaroundtime[i] != -1)
        {
            averageturnaroundtime += turnaroundtime[i];
        }
    }

    int counter = 0;
    for (j = 0; j < count; j++)
    {
        if (timeoverhead[j] != -1.00)
        {
            averagetimeoverhead += timeoverhead[j];
            if (maxturnaroundtime < timeoverhead[j])
            {
                maxturnaroundtime = timeoverhead[j];
            }
            counter++;
        }
    }

    // it was not properly rounding up the maxturnaroundtime
    maxturnaroundtime = roundupdouble(maxturnaroundtime);

    printf("Turnaround time %d\n", (int)ceil(averageturnaroundtime / counter));
    printf("Time overhead %.2f %.2f\n", maxturnaroundtime, averagetimeoverhead / counter);
    printf("Makespan %d\n", time);
}

/* It is fed with time stamp, processid, remaining time of the
   processor and the processor it is currently running.
   Prints out the 'running' command as per specification. */
void print_running(int time, double pid, int remainingtime, int index, char parallelisable)
{
    // typecast for printing in non parallel state - process
    if (parallelisable == 'n')
    {
        printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", time, (int)pid, remainingtime, index);
    }
    else if (parallelisable == 'p')
    {
         // typecast for printing in parallel state - subprocess
        printf("%d,RUNNING,pid=%.1f,remaining_time=%d,cpu=%d\n", time, pid, remainingtime, index);
    }
}

/* It is fed with time stamp, processid and the count of the
   processes remaining.
   Prints out the 'finished' command as per specification. */
void print_finished(int time, int pid, int processremaining)
{
    printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", time, pid, processremaining);
}

/*  Finds the queue size of the existing processes (subprocesses inclusive) waiting on the readyqueue.
    Returns the total size of the sum of processes and subprocesses running. */
int find_queue_size(Queue *readyqueue[], int nprocessor, int unique_subprocess[], int unique_process[])
{
    // variable declaration
    int process_exist = 0;
    int subprocess_exist = 0;
    int process_index = 0;
    int subprocess_index = 0;
    int curr_queue_size = 0;
   
    // go through the readyqueues
    for (int i = 0; i < nprocessor; i++)
    {
        // if readyqueue still has processes
        if (queue_size(readyqueue[i]) > 0)
        {
            Node *node = readyqueue[i]->items->head;
            while (node)
            {
                // if not filled
                if (node->data.parentid == -1)
                {
                    // fill up
                    unique_process[process_index] = node->data.processid;
                    // increase counter
                    process_index++;
                    // set flag to true
                    process_exist = 1;
                }
                else
                {
                    // if subprocess then grab parent pid
                    unique_subprocess[subprocess_index] = node->data.parentid;
                    // increment counter
                    subprocess_index++;
                    // set flag true for subprocess
                    subprocess_exist = 1;
                }
                node = node->next;
            }
        }
    }
    
    // delegate and find the processes waiting in the readyqueue
    if (process_exist == 1)
    {
        curr_queue_size = find_unique_process_subprocess(unique_process, process_index, curr_queue_size);
    }
    
    // delegate and find the subprocesses waiting in the readyqueue
    if (subprocess_exist == 1)
    {
        curr_queue_size = find_unique_process_subprocess(unique_subprocess, subprocess_index, curr_queue_size);
    }
    return curr_queue_size;
}

/* Takes the parent process's data and number of processes = k
   to update the subprocesses' fields.
   Returns the parent process with child encapsulated. */
Data *create_subprocess(Data *dataarray, Data newdata, int k)
{
    for (int i = 0; i < k; i++)
    {
        // copy parent's data to child
        dataarray[i] = newdata; 
        // prev pid becomes parent
        dataarray[i].parentid = (int)newdata.processid;
        // pid in increments of 0.1
        dataarray[i].processid = (newdata.processid + (i * 0.1)); 
        //updated execution time
        dataarray[i].executiontime = (ceil(((double)newdata.executiontime) / k) + 1); 
        // rest copied from parent to child
        dataarray[i].remainingtime = dataarray[i].executiontime;
        dataarray[i].parentexecutiontime = newdata.executiontime;
        dataarray[i].parentarrivaltime = newdata.arrivaltime;
        dataarray[i].parentcompletiontime = newdata.completiontime;
        dataarray[i].parentturnaroundtime = newdata.turnaroundtime;
        dataarray[i].parenttimeoverhead = newdata.timeoverhead;
    }
    return dataarray;
}

/* The program schedules the process to the cpu. 
   Prints out a running statement.
   Returns the queued up cpu (basically a stacked up linked list). */
Queue **schedule_cpu(Queue *cpu[], Data newdata, int time, int index)
{
    // append to cpu at the tail or head if first insertion
    queue_enqueue(cpu[index], newdata);
    // run the print command
    print_running(time, newdata.processid, newdata.remainingtime, index, newdata.parallelisable);
    return cpu;
}

/* Finds the processor with the minimum remaining time. 
   Returns the index of that processor. */
int find_cpu(int nprocessor, Queue *readyqueue[])
{
    // empty cpus given priorities first
    for (int i = 0; i < nprocessor; i++)
    {
        if (queue_size(readyqueue[i]) == 0)
        {
            return i;
        }
    }

    
    int min = INT_MAX;
    int index = 0;
    // for every readyqueue thats filled
    for (int a = 0; a < nprocessor; a++)
    {
        if (queue_size(readyqueue[a]) > 0)
        {
            // loop and find the remaining total remaining time
            Node *node = readyqueue[a]->items->head;
            int totalremainingtime = 0;
            while (node)
            {
                totalremainingtime += node->data.remainingtime;
                node = node->next;
            }
            free(node);
            // only collect the minimum remaining time of the given cpus
            if (min > totalremainingtime)
            {
                min = totalremainingtime;
                index = a;
            }
        }
    }
    return index;
}

/* It removes the finished process from the readyqueue and then stores the 
   relevant informations in their particular array. 
   Prints out a finished statement of the cpu. */
void finished_process(Queue *readyqueue[], int nprocessor, int arrivaltime[], int executiontime[], int completiontime[], int turnaroundtime[], double timeoverhead[], int time, CompressedData *process_done, Data finisheddata)
{
    int curr_queue_size = 0;
    int totalprocesses = totalprocess(readyqueue, nprocessor);
    int unique_subprocess[totalprocesses]; // max queuesize
    int unique_process[totalprocesses];

    if (finisheddata.parallelisable == 'n')
    { 
        // mark off process (done) array
        process_done_processes_non_parallel(process_done, (int)finisheddata.processid, 0);
        
        // store and record of the stats of the  process
        store_stats(arrivaltime, executiontime, completiontime, turnaroundtime, timeoverhead, (int)finisheddata.processid - 1, finisheddata.arrivaltime, finisheddata.executiontime, time);

        // queue size found
        curr_queue_size = find_queue_size(readyqueue, nprocessor, unique_subprocess, unique_process);

        // print off the finished process
        print_finished(time, finisheddata.processid, curr_queue_size);
    }
    else if (finisheddata.parallelisable == 'p')
    {
        // mark off subprocesses (done) array
        process_done_processes_parallel(process_done, (int)finisheddata.processid, finisheddata.parentid);
        //if all children popped off
        if ((process_done[(int)finisheddata.processid].number_child == nprocessor) && (process_done[(int)finisheddata.processid].subprocess == 1))
        {
            // store and record of the stats of the parent process
            store_stats(arrivaltime, executiontime, completiontime, turnaroundtime, timeoverhead, process_done[(int)finisheddata.processid].parentid - 1, finisheddata.parentarrivaltime, finisheddata.parentexecutiontime, time);

            // queue size found of current unfinished subprocesses and processes in readyqueue
            curr_queue_size = find_queue_size(readyqueue, nprocessor, unique_subprocess, unique_process);

            // print off the finished process
            print_finished(time, process_done[(int)finisheddata.processid].parentid, curr_queue_size);
        }
    }
}

/* Checks if any of the readyqueue is empty 
   Returns true if atleast 1 readyqueue is empty. */
int anyemptyqueue(Queue **readyqueue, int nprocessor)
{
    int empty = 0;
    // any readyqueue first encountered as empty returned
    for (int i = 0; i < nprocessor; i++)
    {
        if (queue_size(readyqueue[i]) == 0)
        {
            return 1;
        }
    }
    return empty;
}

/* Set all the elements in the double array to default '-1.00'. */
void initialise_array_defaults_double(double doublearray[], int count)
{
    int i = 0;
    for (i = 0; i < count; i++)
    {
        doublearray[i] = -1.00;
    }
}

/* Set all the elements in the integer array to default '-1'. */
void initialise_array_defaults_integer(int integerarray[], int count)
{
    int i = 0;
    for (i = 0; i < count; i++)
    {
        integerarray[i] = -1;
    }
}

/* Accepts arrays and their length and initialises with respective default values.
  -1 for integer array and -1.00 for double array. */
void initialise_arrays(int arrivaltime[],int executiontime[],int completiontime[],int turnaroundtime[], double timeoverhead[], int count)
{
    // delegation to int array default fill up
    initialise_array_defaults_integer(arrivaltime,count);
    initialise_array_defaults_integer(executiontime,count);
    initialise_array_defaults_integer(completiontime,count);
    initialise_array_defaults_integer(turnaroundtime,count);
    // delegation to double array default fill up
    initialise_array_defaults_double(timeoverhead,count);
}

/* Readins the readinqueue to find the maximum processid.
   Returns maximum processid. */
int find_max_pid(Queue *readinqueue)
{
    Node *node = readinqueue->items->head;
    int maxpid = 0;
    // loop and grab the maximum pid if existant is less
    while (node)
    {
        if (maxpid < (int)node->data.processid)
        {
            maxpid = (int)node->data.processid;
        }
        node = node->next;
    }
    return maxpid;
}

/* Checks if all the readyqueues are empty or not which is indicative
   of whether all processes have been scheduled to the cpu or not.
   Returns a flag in integer of whether the entire readyqueue is empty. */
int allemptyqueue(Queue **readyqueue, int nprocessor)
{
    int emptycount = 0;
    // loop to grab the empty readyqueues (size = 0)
    for (int i = 0; i < nprocessor; i++)
    {
        if (queue_size(readyqueue[i]) == 0)
        {
            emptycount++;
        }
    }
    return emptycount == nprocessor;
}

/* Updates, inserts into cpu the finished processes.
   Returns cpu and readyqueue with the updated insertion. */
Queue **update_readyqueue_cpu(Queue *readyqueue[], Queue *cpu[], int nprocessor, int arrivaltime[], int executiontime[], int completiontime[], int turnaroundtime[], double timeoverhead[], int time, CompressedData *process_done)
{
    int process_pop = 0;

    // loop and update the remaining time of readyqueues
    for (int t = 0; t < nprocessor; t++)
    {
        if (queue_size(readyqueue[t]) != 0)
        {
            readyqueue[t]->items->head->data.remainingtime -= 1;
        }
    }

    // check for when the processes finish and collect them, handy for collecting all 
    // simultaneous finishes
    Data* processes_completed = NULL;
    int insertion = 0;
    for (int t = 0; t < nprocessor; t++)
    {
        if(queue_size(readyqueue[t])!=0)
        {
            if (readyqueue[t]->items->head->data.remainingtime == 0)
            {
                Data finisheddata = queue_dequeue(readyqueue[t]);
                // allocate fresh memories to collect completed processes
                processes_completed = realloc(processes_completed, sizeof(Data)*(insertion+1));
                processes_completed[insertion] = finisheddata;
                // know how much finished for later looping
                insertion++;
                // flag to indicate processes finished
                process_pop = 1;
            }
        }
    }
    
    // check if there are processes finished, then collect stats and print
    if(process_pop==1)
    {
        // calculate, store and mark of processes done
        for(int u= 0; u < insertion;u++)
        {
            // delegation to finished process
            finished_process(readyqueue, nprocessor, arrivaltime, executiontime, completiontime, turnaroundtime, timeoverhead, time, process_done, processes_completed[u]);
        }
        // must free up the space used to track processes done
        free(processes_completed);
    }
    
    // schedule tasks for the processes that are assigned
    for (int t = 0; t < nprocessor; t++)
    {
        if (queue_size(readyqueue[t]) > 0)
        {
            // if the popped up process is different to the running one in cpu
            // must use the head as cpu has one element inserted
            if((queue_size(cpu[t])== 1) && (cpu[t]->items->head->data.remainingtime != readyqueue[t]->items->head->data.remainingtime) && (cpu[t]->items->head->data.processid != readyqueue[t]->items->head->data.processid))
            {
                queue_enqueue(cpu[t], readyqueue[t]->items->head->data);
                print_running(time, cpu[t]->items->tail->data.processid, cpu[t]->items->tail->data.remainingtime, t, cpu[t]->items->tail->data.parallelisable);
            }
            // must use the head as cpu has more than one element inserted
            else if ((queue_size(cpu[t])> 1) && (cpu[t]->items->tail->data.remainingtime != readyqueue[t]->items->head->data.remainingtime) && (cpu[t]->items->tail->data.processid != readyqueue[t]->items->head->data.processid))
            {
                queue_enqueue(cpu[t], readyqueue[t]->items->head->data);
                print_running(time, cpu[t]->items->tail->data.processid, cpu[t]->items->tail->data.remainingtime, t, cpu[t]->items->tail->data.parallelisable);
            }
        }
    }
    return cpu;
}

/* Fills in  a readyqueue and the assigns a process to cpu if empty or if 
   the process has lesser remaining time than the existing process running 
   in the cpu. 
   The cpu is then returned. */
Queue **store_assign_task(Queue *cpu[], Queue *readyqueue[], int time, int nprocessor, Data newdata)
{
    // find the least remaining time (readqueue id correspondence to 
    // cpu id preferred over similar remaining time)
    int index = find_cpu(nprocessor, readyqueue);

    // if readyqueue is empty, fill and schedule the process
    if (queue_size(readyqueue[index]) == 0)
    {
        queue_enqueue(readyqueue[index], newdata);
        cpu = schedule_cpu(cpu, readyqueue[index]->items->head->data, time, index);
    }
    else
    { 
        // if readyqueue is already filled then track previous data value, sort
        // compare with the enqueued node on the readyqueue
        Data prevdata = readyqueue[index]->items->head->data;
        queue_enqueue(readyqueue[index], newdata);
        bubbleSort(readyqueue[index]);

        // a lesser remaining time process with different pid is allowed for a switch on the cpu
        if (prevdata.remainingtime != readyqueue[index]->items->head->data.remainingtime &&prevdata.processid != readyqueue[index]->items->head->data.processid)
        {
            cpu = schedule_cpu(cpu, readyqueue[index]->items->head->data, time, index);
        }
    }
    return cpu;
}

/* No process exchange occurs on the ongoing cpu from the readyqueue.The processes
   continue till it gets exhausted (it's remaining time becomes zero for a particular process).
   Returns the cpu once an update at either readyqueue or cpu(scheduled a task). */
Queue **fcfs(Queue *cpu[], Queue *readyqueue[], int time, int totalexecutiontime, int *nprocessor, int arrivaltime[], int executiontime[], int completiontime[], int turnaroundtime[], double timeoverhead[], CompressedData *process_done)
{
    int process_pop = 0;

    // simulatenously decrease all remaining time of the cpus
    for (int i = 0; i < *nprocessor; i++)
    {
        if (queue_size(readyqueue[i]) > 0)
        {
            readyqueue[i]->items->head->data.remainingtime -= 1;
        }
    }
    
    // check for when the processes finish and collect them, handy for collecting all 
    // simultaneous finishes
    Data* processes_completed = NULL;
    int insertion = 0;
    for (int i = 0; i < *nprocessor; i++)
    {
        if(queue_size(readyqueue[i])!=0)
        {
            if(readyqueue[i]->items->head->data.remainingtime == 0)
            {
                Data finisheddata = queue_dequeue(readyqueue[i]);
                // allocate fresh memories to collect completed processes
                processes_completed = realloc(processes_completed, sizeof(Data)*(insertion+1));
                processes_completed[insertion] = finisheddata;
                // know how much finished for later looping
                insertion++;
                // flag to indicate processes finished
                process_pop = 1;
            }
        }
    }

    // check if there are processes finished, then collect stats and print
    if(process_pop==1)
    {
        // calculate, store and mark of processes done
        for(int u= 0; u < insertion;u++)
        {
            finished_process(readyqueue, *nprocessor, arrivaltime, executiontime, completiontime, turnaroundtime, timeoverhead, time, process_done, processes_completed[u]);
        }
        // must free up the space used to track processes done
        free(processes_completed);
    }
    
    // if all processes finish - safe exit
    if (allemptyqueue(readyqueue, *nprocessor))
    {
        return cpu;
    }
    
    // schedule tasks for the processes that are assigned 
    for (int i = 0; i < *nprocessor; i++)
    {
        if(queue_size(readyqueue[i]) > 0)
        {
            // if the popped up process is different to the running one in cpu
            if(cpu[i]->items->tail->data.processid!=readyqueue[i]->items->head->data.processid)
            {
               cpu = schedule_cpu(cpu, readyqueue[i]->items->head->data, time, i); 
            }
        }
    }
    return cpu;
}

/* This program handles the scenario when multiple process arrives from the readinqueue at 
   the same particular time. The processes are collected at the empty least remaining time 
   readyqueue corresponding to the equivalent cpu. For a multiprocessor, it goes to the 
   least remaining time readyqueue which corresponds to an equivalent least remaining time cpu.
   Returns the queued in cpu from the readyqueue. */
Queue **multiarrival(Queue *readinqueue, int time, Queue *readyqueue[], Queue *cpu[], int *nprocessor, int arrivaltime[], int executiontime[], int completiontime[], int turnaroundtime[], double timeoverhead[], int multi, CompressedData *process_done)
{
    
    int insertion = 0;
    while (time == readinqueue->items->head->data.arrivaltime)
    {
        if (readinqueue->items->head->data.parallelisable == 'n')
        {
            // remove the non parallel process from readinqueue
            // find the cpu and then insert into the readyqueue
            Data newdata = queue_dequeue(readinqueue);
            int index = find_cpu(*nprocessor, readyqueue);
            queue_enqueue(readyqueue[index], newdata);
            insertion++;
        }
        else if (readinqueue->items->head->data.parallelisable == 'p')
        {
            // allocate the array to copy to readyqueue
            Data *dataarray = malloc(sizeof(Data) * (*nprocessor));
            // remove each parallelised (thread) process,convert to subprocess
            // and then insert  into the readyqueue 
            Data newdata = queue_dequeue(readinqueue);
            dataarray = create_subprocess(dataarray, newdata, *nprocessor);
            for (int i = 0; i < *nprocessor; i++)
            {
                int index = find_cpu(*nprocessor, readyqueue);
                queue_enqueue(readyqueue[index], dataarray[i]);
                insertion++;
            }
            free(dataarray);
        }
        // stop collecting when all processes are in readyqueue
        if (queue_size(readinqueue) == 0)
        {
            break;
        }
    }
    
    for (int i = 0; i < *nprocessor; i++)
    { 
        // assign to any empty queue - all empty queues state here
        if(queue_size(cpu[i])==0)
        {
                cpu = schedule_cpu(cpu, readyqueue[i]->items->head->data, time,i);
        }
        else if (queue_size(cpu[i])==1)
        {
             // assign to any empty queue - atleast one filled empty queue state here
            if((cpu[i]->items->head->data.remainingtime != readyqueue[i]->items->head->data.remainingtime) && (cpu[i]->items->head->data.processid != readyqueue[i]->items->head->data.processid))
            {
                cpu = schedule_cpu(cpu, readyqueue[i]->items->head->data, time,i);
            }
        }
        else if (queue_size(cpu[i])>1)
        {
            // assign when all are assigned some process before   
            if((cpu[i]->items->tail->data.remainingtime != readyqueue[i]->items->head->data.remainingtime) && (cpu[i]->items->tail->data.processid != readyqueue[i]->items->head->data.processid)){
                cpu = schedule_cpu(cpu, readyqueue[i]->items->head->data, time,i);
            }
        }
    }

    return cpu;
}

/* This program handles the scenario when a single process arrives from the readinqueue.
   The processes go to the readyqueue corresponding to the equivalent cpu.
   For a multiprocessor, it goes to the least remaining time readyqueue which corresponds to
   an equivalent least remaining time cpu.
   Returns the queued in cpu from the readyqueue. */
Queue **singlearrival(Queue *readinqueue, int time, Queue *readyqueue[], Queue *cpu[], int *nprocessor, int arrivaltime[], int executiontime[], int completiontime[], int turnaroundtime[], double timeoverhead[], CompressedData *process_done)
{
    // remove a node 
    Data newdata = queue_dequeue(readinqueue);
    
    // after time > 0 some processes are running, update both readyqueue and cpu
    if (time > 0)
    {
        cpu = update_readyqueue_cpu(readyqueue, cpu, *nprocessor, arrivaltime, executiontime, completiontime, turnaroundtime, timeoverhead, time, process_done);
    }
    // delegate task and store and assign each process from readyqueue to cpu
    if (newdata.parallelisable == 'n')
    {
        cpu = store_assign_task(cpu, readyqueue, time, *nprocessor, newdata);
    }
    else if (newdata.parallelisable == 'p')
    {
        // store, delegate and assign cpu by creating, copying subprocesses onto an array
        // free memory once completed 
        Data *dataarray = malloc(sizeof(Data) * (*nprocessor));
        dataarray = create_subprocess(dataarray, newdata, *nprocessor);

        for (int i = 0; i < *nprocessor; i++)
        {
            cpu = store_assign_task(cpu, readyqueue, time, *nprocessor, dataarray[i]);
        }
        free(dataarray);
    }
    return cpu;
}

/* Shortest remaining time first algorithm implementation done here.
   With progress of time, it stores the processes in a ready queue
   and whenever any incoming process finds a spot, it overtakes the cpu.
   Later the statistics of the simulation printed out. */
void strf(int *nprocessor, Queue *readinqueue, int totalexecutiontime, int count)
{
    // strf implementation starts here
    int time = 0;
    int multi = 0;
    int arrivaltime[count];
    int executiontime[count];
    int completiontime[count];
    int turnaroundtime[count];
    double timeoverhead[count];
    Queue **readyqueue = malloc(sizeof(Queue*) * (*nprocessor));
    Queue **cpu = malloc(sizeof(Queue*) * (*nprocessor));
    int maxpid = find_max_pid(readinqueue);
    CompressedData *process_done = malloc(sizeof(CompressedData) * (maxpid+1));
   
    // assign the relevant temporary arrays
    initialise_arrays(arrivaltime, executiontime, completiontime, turnaroundtime,timeoverhead, count);
    
    // initialise the handy process done to keep track of subprocesses
    initialise_process_done(process_done,maxpid+1);

     // generate processors as requested
    for (int i = 0; i < (*nprocessor); i++)
    {
        cpu[i] = new_queue();
        readyqueue[i] = new_queue();
    }

    // preprocessing phase
    if (readinqueue->items->head->data.arrivaltime == readinqueue->items->head->next->data.arrivaltime)
    {
        bubbleSort(readinqueue);
        multi = 1;
    }

    // Main brain of the strf algorithm.
    // The readinqueue must have read some input or the readyqueue must have gone through
    // all the assignments to the cpu.
    while (queue_size(readinqueue) > 0 || !(allemptyqueue(readyqueue, *nprocessor)))
    {
        // check if the processes arent assigned
        if (queue_size(readinqueue) > 0)
        {
            // assign upon arrival
            if (time == readinqueue->items->head->data.arrivaltime)
            {
                // if readinqueue has atleast two elements
                if (readinqueue->items->head->next != NULL)
                {
                    // atleast 2 consecutive processes have the same arrival time
                    if (time == readinqueue->items->head->data.arrivaltime && time == readinqueue->items->head->next->data.arrivaltime && multi == 1)
                    {
                        cpu = multiarrival(readinqueue, time, readyqueue, cpu, nprocessor, arrivaltime, executiontime, completiontime, turnaroundtime, timeoverhead, multi, process_done);
                    }
                    else  if (time == readinqueue->items->head->data.arrivaltime && time != readinqueue->items->head->next->data.arrivaltime)
                    { 
                        // one process arrives while the other is on its way
                        cpu = singlearrival(readinqueue, time, readyqueue, cpu, nprocessor, arrivaltime, executiontime, completiontime, turnaroundtime, timeoverhead, process_done);
                    }
                }
                else
                {  
                    // just one process left in readyqueue to assign to cpu
                    cpu = singlearrival(readinqueue, time, readyqueue, cpu, nprocessor, arrivaltime, executiontime, completiontime, turnaroundtime, timeoverhead, process_done);
                }
            }
            else 
            {
                // update on the running processes
                cpu = update_readyqueue_cpu(readyqueue, cpu, *nprocessor, arrivaltime, executiontime, completiontime, turnaroundtime, timeoverhead, time, process_done);
            }
        }
        else if (queue_size(readinqueue) == 0)
        {
            // all processes are assign and it then goes through a simple fcfs
            cpu = fcfs(cpu, readyqueue, time, totalexecutiontime, nprocessor, arrivaltime, executiontime, completiontime, turnaroundtime, timeoverhead, process_done);
        }
        // update time when all steps passed
        time++;
    }

    // all necessary statistics are calculated
    printoutstats(count, *nprocessor, turnaroundtime, timeoverhead, time - 1);

    // free up memory
    free_queue(readinqueue);
    
     for (int i = 0; i < (*nprocessor); i++)
     {
         free_queue(cpu[i]);
         free_queue(readyqueue[i]);
     }
    free(readyqueue);
    free(cpu);
    free(process_done);
}

/* Control centre of the program that delegates tasks to relevant functions.
   Count of arguments from stdin stored in argc with argv storing the entire
   string from stdin.
   Returns 0 on successful exit. */
int main(int argc, char *argv[])
{
    /* Variables declared */
    char *filepath = NULL;
    int nprocessor = 0;
    int count = 0;
    int totalexecutiontime = 0;
    int challenge = 0;
    Queue *readinqueue = NULL;

    // read from the stdin
    filepath = readfromstdin(argc, argv, &nprocessor, filepath, &challenge);

    // read the file on to a queue
    readinqueue = readfile(filepath, readinqueue, &totalexecutiontime, &count, challenge);

    // create a function for strf here
    strf(&nprocessor, readinqueue, totalexecutiontime, count);

    return 0;
}