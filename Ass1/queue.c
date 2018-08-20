#include <stdlib.h>
#include <assert.h>
#include "queue.h"
#include "list.h"


/* * * *
 * FUNCTION DEFINITIONS
 */

 List*
 new_queue(){
   return (new_list());
 }


 void
 free_queue(List* list) {
   free_list(list);
 }


 void
 queue_enqueue(List* list, int data) {
   list_add_end(list, data);
 }

int
queue_dequeue(List* list){
    return(list_remove_start(list));
}

int
queue_size(List* list){
    return(list_size(list));
}
