/* * * * * * *
 * Header file for functions that solve the coding tasks from assignment 1
 *
 * created for COMP20007 Design of Algorithms - Assignment 1, 2017
 * by Matt Farrugia <matt.farrugia@unimelb.edu.au>
 */

#ifndef TRAVERSE_H
#define TRAVERSE_H

#include "graph.h"
#include "stack.h"

// see Assignment Part 1: Depth-First Traversal
void print_dfs(Graph* graph, int source_id);

// see Assignment Part 2: Breadth-First Traversal
void print_bfs(Graph* graph, int source_id);

// see Assignment Part 3: Finding a Detailed Path
void detailed_path(Graph* graph, int source_id, int destination_id);

// see Assignment Part 4: Finding All Paths
void all_paths(Graph* graph, int source_id, int destination_id);

// see Assignment Part 5: Finding the Shortest Path
void shortest_path(Graph* graph, int source_id, int destination_id);

/*New functions defined*/
/*Used for part 4 and 5 respectively*/
void all_paths_print(Graph* graph, int start, int end, List* current_path_list, int accessed[]);

void print_shortest_path(Graph* graph, int start, int end, List* current_path_list, int accessed[],int* shortest_dist, int current_dist, List* current_shortest_path_list);


#endif
