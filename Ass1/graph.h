/* * * * * * *
 * Module for creating and manipulating weighted graphs
 *
 * created for COMP20007 Design of Algorithms - Assignment 1, 2017
 * by Matt Farrugia <matt.farrugia@unimelb.edu.au>
 */

#ifndef GRAPH_H
#define GRAPH_H

typedef struct graph Graph;
typedef struct vertex Vertex;
typedef struct edge Edge;

// a graph knows its order (number of vertices) and an array of pointers to
// those vertices.
// these values can be used, but should not be *modified* outside of graph.c.
// they are read-only!
struct graph {
	int n, maxn;
	Vertex** vertices;
};

// a vertex has a label and a pointer to the first edge in its adjacency list.
// these values can be used, but should not be *modified* outside of graph.c.
// they are read-only!

struct vertex {
	char* label;
	Edge* first_edge;
};

// an edge knows the IDs of its two incident vertices; from u, to v
// each edge also knows its weight, and points to the next edge in a list of
// edges from the same vertex (or to NULL if it's the last edge in the list).
// these values can be used, but should not be *modified* outside of graph.c.
// they are read-only!
struct edge {
	int u, v;
	int weight;
	Edge* next_edge;
};

// create a new, empty graph, with space for a maximum of n vertices
Graph* new_graph(int n);

// destroy a graph, its vertices, and their edges
void free_graph(Graph* graph);


// add a new vertex with label 'name' to a graph
void graph_add_vertex(Graph* graph, const char* name);

// add an undirected edge between u and v with weight w to graph
void graph_add_u_edge(Graph* graph, int u, int v, int w);

// add a directed edge from u to v with weight w to a graph
void graph_add_d_edge(Graph* graph, int u, int v, int w);

#endif
