/* * * * * * *
 * Module for creating and manipulating weighted graphs
 *
 * created for COMP20007 Design of Algorithms - Assignment 1, 2017
 * by Matt Farrugia <matt.farrugia@unimelb.edu.au>
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "graph.h"

/* helper function prototypes */

// create a new vertex with a specific label
Vertex* new_vertex(const char* name);

// create a new w-weighted edge from vertex id u to vertex id v
Edge* new_edge(int u, int v, int w);

// destroy a vertex, including its label and all of its edges
void free_vertex(Vertex* vertex);


/* function definitions */

// create a new, empty graph, with space for n vertices
Graph* new_graph(int n) {
	Graph* graph = malloc(sizeof (*graph));
	assert(graph);

	graph->n = 0;
	graph->maxn = n;

	graph->vertices = malloc(n * sizeof (Vertex*));
	assert(graph->vertices);

	return graph;
}

// create a new vertex with a specific label
Vertex* new_vertex(const char* label) {
	assert(label);

	Vertex* vertex = malloc(sizeof (*vertex));
	assert(vertex);

	// make sure to copy the label across
	vertex->label = malloc((1 + strlen(label)) * sizeof (char));
	assert(vertex->label);
	strcpy(vertex->label, label);

	vertex->first_edge = NULL;


	return vertex;
}

// create a new w-weighted edge from vertex id u to vertex id v
Edge* new_edge(int u, int v, int w) {
	Edge* edge = malloc(sizeof (*edge));
	assert(edge);

	edge->u = u;
	edge->v = v;
	edge->weight = w;

	edge->next_edge = NULL;

	return edge;
}

// destroy a graph, its vertices, and their edges
void free_graph(Graph* graph) {
	if (graph) {
		int i;
		for (i = 0; i < graph->n; i++) {
			free_vertex(graph->vertices[i]);
		}

		free(graph->vertices);
		free(graph);
	}
}

// destroy a vertex, including its label and all of its edges
void free_vertex(Vertex* vertex) {
	if (vertex) {
		while (vertex->first_edge) {
			Edge* edge = vertex->first_edge;
			vertex->first_edge = vertex->first_edge->next_edge;
			free(edge);
		}
		free(vertex->label);
		free(vertex);
	}
}

// add a new vertex with label 'name' to a graph
void graph_add_vertex(Graph* graph, const char* name) {
	if (graph->n < graph->maxn) {
		graph->vertices[graph->n] = new_vertex(name);
		graph->n++;
	} else {
		fprintf(stderr, "hey! adding new vertex to full graph\n");
	}
}

// add an undirected edge between u and v with weight w to graph
void graph_add_u_edge(Graph* graph, int u, int v, int w) {
	// an undirected edge is just two directed edges
	graph_add_d_edge(graph, u, v, w);
	graph_add_d_edge(graph, v, u, w);
}

// add a directed edge from u to v with weight w to a graph
void graph_add_d_edge(Graph* graph, int u, int v, int w) {
	if(u < graph->n && u >= 0 && v < graph->n && v >= 0) {
		Edge* edge = new_edge(u, v, w);
		edge->next_edge = graph->vertices[u]->first_edge;
		graph->vertices[u]->first_edge = edge;
	} else {
		fprintf(stderr, "hey! adding edge between non-existant vertices\n");
	}
}
