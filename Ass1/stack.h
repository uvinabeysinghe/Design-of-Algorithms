/* * * * * * *
 * Module for creating and manipulating stacks of integers
 *
 * created for COMP20007 Design of Algorithms 2017
 *
 */


 #include <stdbool.h>
 #include "list.h"


// create a new stack and return a pointer to it
List* new_stack();

// destroy a stack and free its memory
void free_stack(List* list);

// add an element to the stack
// this operation is O(1)
void stack_push(List* list, int data);

// remove and return an element from the stack
// this operation is O(1)
// error if the stack is empty (so first ensure stack_size() > 0)
int stack_pop(List* list);

// return the number of elements contained in a stack
int stack_size(List* list);
