/* * * * * * *
 * Module for creating and manipulating queues of integers
 *
 * created for COMP20007 Design of Algorithms 2017
 *
 */


 #include <stdbool.h>
 #include "list.h"


// create a new queue and return a pointer to it
List* new_queue();

// destroy a queue and free its memory
void free_queue(List* list);

// add an element to the queue
// this operation is O(1)
void queue_enqueue(List* list, int data);

// remove and return an element from the queue
// this operation is O(1)
// error if the queue is empty (so first ensure queue_size() > 0)
int queue_dequeue(List* list);

// return the number of elements contained in a queue
int queue_size(List* list);
