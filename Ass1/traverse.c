/*Functions written by Uvin Abeysinghe
 *Student Number: 789931
 *For design of algorithm 2017 sem 1
 *Algorithmns are fun
 */


#include <stdio.h>
#include "traverse.h"
#include "stack.h"
#include "queue.h"
#include "list.h"


/******************************************************************************/
/*Function prototype*/

void print_shortest_path(Graph* graph, int start, int end,
	List* current_path_list, int accessed[],int* shortest_dist,
	int current_dist, List* current_shortest_path_list) ;

void all_paths_print(Graph* graph, int start, int end, List* current_path_list,
	 int accessed[]) ;

/******************************************************************************/

void print_dfs(Graph* graph, int source_id) {

	/*Declaring and initializing an integer array which will hold 1 if visited
	 *or will hold 0 if unvisited, array index corresponds to the vertex index
	 */
	int i;
	int accessed[graph->n];
	for(i=0;i<graph->n;i++){
		accessed[i]=0;
	}

	/*Declaring variables needed*/
	Edge* temp_edge;
	struct vertex* s_vertex= graph->vertices[source_id];
	struct vertex* temp_vertex;


	/*Creating an empty stack and noting the starting vertex as visited and
	 *pushing the starting vertex into the stack
	 */
	List* depth_stack=new_stack();
	accessed[source_id]=1;
	temp_vertex=s_vertex;
	printf("%s\n", temp_vertex->label);
	stack_push(depth_stack, source_id);

	/*loop until the stack is empty*/
	while (stack_size(depth_stack)!=0){
		temp_edge=temp_vertex->first_edge;
		/*loop until temp_edge is NULL*/
		while (temp_edge){
			/*checking if the vertex pointed by the current edge is visited*/
			if (accessed[temp_edge->v]){
				/*moving to the next edge of the current vertex*/
				temp_edge=temp_edge->next_edge;
				/*if no edges left in the current vertex, go back one level in
				 *the stack to the previous vertex
				 */
				if(!temp_edge){
					stack_pop(depth_stack);
				}
			}else{
				/*if we have not visited the vertex, the vertex index is pushed
				 *into the stack, is printed and noted as visited
				 */
				stack_push(depth_stack,temp_edge->v);
				printf("%s\n", graph->vertices[temp_edge->v]->label);
				accessed[temp_edge->v]=1;
				/*change the temp_vertex to the new vertex pushed and current
				 *edge is set to null
				 */
				temp_vertex=(graph->vertices[temp_edge->v]);
				temp_edge=NULL;
			}
		}
	}
	free_stack(depth_stack);
}

/******************************************************************************/
void print_bfs(Graph* graph, int source_id) {

	/*Declaring and initializing an integer array which will hold 1 if visited
	 *or will hold 0 if unvisited, array index corresponds to the vertex index
	 */
	int i;
	int accessed[graph->n];
	for(i=0;i<graph->n;i++){
		accessed[i]=0;
	}

	/*Declaring variables needed*/
	Edge* temp_edge;
	struct vertex* s_vertex= graph->vertices[source_id];
	struct vertex* temp_vertex;

	/*Creating an empty queue and noting the starting vertex as visited and
	 *enqueued the starting vertex into the queue. Prints out the starting point
	 *label
	 */
	List* breadth_queue=new_queue();
	accessed[source_id]=1;
	temp_vertex=s_vertex;
	printf("%s\n", temp_vertex->label);
	queue_enqueue(breadth_queue, source_id);

	/*loop until the queue is empty*/
	while (queue_size(breadth_queue)){
		temp_edge=temp_vertex->first_edge;

		/*loop until temp_edge is NULL*/
		while (temp_edge){
			/*checking if the vertex pointed by the current edge is visited*/
			if (accessed[temp_edge->v]){
				/*moving to the next edge of the current vertex*/
				temp_edge=temp_edge->next_edge;
				/*if no edges left in the current vertex, we dequeue the queue
				 *and get the next vertex to be processed
				 */
				if(!temp_edge){
					temp_vertex=graph->vertices[queue_dequeue(breadth_queue)];
					temp_edge=NULL;

				}
			}else{
				/*if we have not visited the vertex, the vertex index is pushed
				 *into the queue, is printed and noted as visited.
				 */
				queue_enqueue(breadth_queue,temp_edge->v);
				printf("%s\n", graph->vertices[temp_edge->v]->label);
				accessed[temp_edge->v]=1;
				temp_edge=temp_edge->next_edge;
			}
		}
	}
	free_queue(breadth_queue);

}

/******************************************************************************/

void detailed_path(Graph* graph, int source_id, int destination_id) {

	/*Declaring and initializing an integer array which will hold 1 if visited
	 *or will hold 0 if unvisited, array index corresponds to the vertex index
	 */
	int i;
	int accessed[graph->n];
	for(i=0;i<graph->n;i++){
		accessed[i]=0;
	}

	/*Declaring variables needed*/
	int cumulative_distance=0;

	/*Creating an empty list to save the distance from each vertex to vertex and
	 *initialises variables needed
	 */
	List* distance=new_list();
	list_add_end(distance, 0);
	Edge* temp_edge;
	struct vertex* s_vertex= graph->vertices[source_id];
	struct vertex* temp_vertex;
	/*Creating an empty list and noting the starting vertex as visited and
	 *adding the the starting vertex at the end of the list.
	 */
	List* detailed_path_list=new_list();
	accessed[source_id]=1;
	temp_vertex=s_vertex;
	list_add_end(detailed_path_list, source_id);

	/*loop until the queue is empty*/
	while (list_size(detailed_path_list)){
		temp_edge=temp_vertex->first_edge;

		/*loop until temp_edge is NULL*/
		while (temp_edge){

			/*checking if the vertex pointed by the current edge is visited*/
			if (accessed[temp_edge->v]){
				temp_edge=temp_edge->next_edge;

				/*if no edges left in the current vertex, we remove from the
				 *back of the list the next vertex and distance
				 */
				if(!temp_edge){
					list_remove_end(detailed_path_list);
					list_remove_end(distance);
				}
			}else{
				/*if we have not visited the vertex, the vertex index is added to
				 * the end of the list, is printed and noted as visited.
				 *the distance of the edge used is added as well.
				 */
				list_add_end(detailed_path_list,temp_edge->v);
				list_add_end(distance,temp_edge->weight);
				accessed[temp_edge->v]=1;
				temp_vertex=(graph->vertices[temp_edge->v]);
				/*if the current edge is pointing to the searched destination,
				 *the path is printed while the list is removed from the start,
				 *the distances are added/printed while removing from the start
				 */
				if (temp_edge->v==destination_id){
					while (list_size(detailed_path_list)){
						cumulative_distance=
						    cumulative_distance+list_remove_start(distance);
						printf("%s (%dkm)\n",graph->vertices[list_remove_start
						(detailed_path_list)]->label, cumulative_distance);
					}
				}
				temp_edge=NULL;
			}
		}
	}



free_list(distance);
free_list(detailed_path_list);
}

/******************************************************************************/
/*used the algorithmn from*/
/* http://www.geeksforgeeks.org/find-paths-given-source-destination/ */

void all_paths_print(Graph* graph, int start, int end, List* current_path_list,
	 int accessed[]) {
	int i, j;
	/*setting that the starting vertex is visited and adding it to the
	 *end of the list.
	 */
	accessed[start]=1;
	list_add_end(current_path_list, start);
	/*if the start and end values passed through are the same*/
	if (start==end){
		/*prints out the current path list_size from the start
		 */
		for (i=0; i<(list_size(current_path_list)); i++){
			j=list_remove_start(current_path_list);
			printf("%s", graph->vertices[j]->label);
			if (i<(list_size(current_path_list))){
				printf(", ");
			}
			list_add_end(current_path_list, j);

		}
		printf("\n");
	}else{
		/*if destination not found, a loop through all the edges of the current
		 *vertex
		 */
		Edge* temp_edge=graph->vertices[start]->first_edge;
		while (temp_edge){
			/*if the vertex pointed is not accesed, recurse with the new start
			 *vertex*/
			if (!accessed[temp_edge->v]){
				all_paths_print(graph,temp_edge->v,end, current_path_list,
					 accessed);
			}
			temp_edge=temp_edge->next_edge;

		}


	}
	list_remove_end(current_path_list);
	accessed[start]=0;
}

void all_paths(Graph* graph, int source_id, int destination_id) {
	int i;
	/*Declaring and initializing an integer array which will hold 1 if visited
	 *or will hold 0 if unvisited, array index corresponds to the vertex index
	 */
	int accessed[graph->n];
	for(i=0;i<graph->n;i++){
		accessed[i]=0;
	}
	/*Declaring a list to store the current path traversed
	 */
	List* current_path_list=new_list();


	all_paths_print(graph, source_id, destination_id, current_path_list,
		 accessed);
	free_list(current_path_list);
}

/******************************************************************************/


void print_shortest_path(Graph* graph, int start, int end,
	List* current_path_list, int accessed[],int* shortest_dist,
	int current_dist, List* current_shortest_path_list) {
	int i, j;
	/*setting that the starting vertex is visited and adding it to the
	 *end of the list.
	 */
	accessed[start]=1;
	list_add_end(current_path_list, start);

	/*if the start and end values passed through are the same*/
	if (start==end){
		/*checking of the current distance is smaller than the shortest
		 */
		if (current_dist<*shortest_dist){
			/*Emptying the shortest path list
			 */
			while (list_size(current_shortest_path_list)){
				list_remove_start(current_shortest_path_list);
			}
			/*copying the current path list to the shortest path list
			 */
			for (i=0; i<(list_size(current_path_list)); i++){
				j=list_remove_start(current_path_list);
				list_add_end(current_shortest_path_list, j);
				list_add_end(current_path_list, j);

			}

			*shortest_dist=current_dist;

		}
	}else{
		/*if destination not found, a loop through all the edges of the current
		 *vertex
		 */
		Edge* temp_edge=graph->vertices[start]->first_edge;
		while (temp_edge){
			/*if the vertex pointed is not accesed, recurse with the new start
			 *vertex*/
			/*Each node distances added and removed
			 */
			if (!accessed[temp_edge->v]){
				current_dist=current_dist+temp_edge->weight;
				print_shortest_path(graph,temp_edge->v,end, current_path_list,
					 accessed, shortest_dist, current_dist,
					  current_shortest_path_list);
				current_dist=current_dist-temp_edge->weight;

			}
			temp_edge=temp_edge->next_edge;

		}


	}
	list_remove_end(current_path_list);
	accessed[start]=0;
}

void shortest_path(Graph* graph, int source_id, int destination_id) {
	int shortest_dist=1000000000;
	int current_dist=0;
	int i;

	/*Declaring and initializing an integer array which will hold 1 if visited
	 *or will hold 0 if unvisited, array index corresponds to the vertex index
	 */
	int accessed[graph->n];
	for(i=0;i<graph->n;i++){
		accessed[i]=0;
	}

	/*Declaring a list to store the current path traversed
	 *Declaring a lsit tostore the shortest path
	  */

	List* current_path_list=new_list();
	List* current_shortest_path_list=new_list();


	print_shortest_path(graph, source_id, destination_id, current_path_list,
		accessed, &shortest_dist, current_dist, current_shortest_path_list);
	/*printing out the shortest path from the current_shortest_path_list
	 */
	while (list_size(current_shortest_path_list)){
		printf("%s", graph->vertices[list_remove_start(
			current_shortest_path_list)]->label);
			if ((list_size(current_shortest_path_list))){
				printf(", ");
			}
	}
	printf(" (%dkm)\n",shortest_dist);
	/*free the list*/
	free_list(current_path_list);
	free_list(current_shortest_path_list);

}
