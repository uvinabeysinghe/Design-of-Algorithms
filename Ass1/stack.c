#include <stdlib.h>
#include <assert.h>
#include "queue.h"
#include "list.h"


/* * * *
 * FUNCTION DEFINITIONS
 */

 List*
 new_stack(){
   return (new_list());
 }


 void
 free_stack(List* stack) {
   free_list(stack);
 }


 void
 stack_push(List* stack, int data) {
   list_add_end(stack, data);
 }

int
stack_pop(List* stack){
    return(list_remove_end(stack));
}

int
stack_size(List* stack){
    return(list_size(stack));
}
