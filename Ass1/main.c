/* * * * * * * * *
 * Main program:
 * reads command line options, builds graph, calls traversal code
 *
 * created for COMP20007 Design of Algorithms - Assignment 1, 2017
 * by Matt Farrugia <matt.farrugia@unimelb.edu.au>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>

#include "graph.h"
#include "traverse.h"

#define MAX_LINE_LEN 256

// enum of all of the parts in order of their numbers
typedef enum part {
	PRINT_DFS = 1, PRINT_BFS, DETAILED_PATH, ALL_PATHS, SHORTEST_PATH,
} Part;

// struct to store the command line options
typedef struct options {
	Part part;
	int source, dest;
	char* filename;
} Options;


Options get_options(int argc, char** argv);
Graph* read_graph(char* filename);
void validate_vertex_ids(Options options, int graph_size);


int main(int argc, char** argv) {
	
	// parse command line options
	Options options = get_options(argc, argv);

	// build graph from data file
	Graph* graph = read_graph(options.filename);

	// validate vertex ids now that we know graph size
	validate_vertex_ids(options, graph->n);

	// branch to relevant function depending on execution mode
	switch (options.part) {
		case PRINT_DFS:
			print_dfs(graph, options.source);
			break;
		case PRINT_BFS:
			print_bfs(graph, options.source);
			break;
		case DETAILED_PATH:
			detailed_path(graph, options.source, options.dest);
			break;
		case ALL_PATHS:
			all_paths(graph, options.source, options.dest);
			break;
		case SHORTEST_PATH:
			shortest_path(graph, options.source, options.dest);
			break;
	}

	// clean up
	free_graph(graph);

	// done!
	exit(EXIT_SUCCESS);
}

// read command line options into Options struct
// prints usage tips and exits on missing arguments
Options get_options(int argc, char** argv) {
	
	// create the Options structure with defaults
	Options options = {.part = 0, .source = -1, .dest = -1, .filename = NULL};

	// use C's built-in getopt function to scan inputs by flag
	char input;
	while ((input = getopt(argc, argv, "p:s:d:f:")) != EOF){
		switch (input){
			case 'p': // set part
				options.part = atoi(optarg);
				break;
			case 's': // set sourceing vertex
				options.source = atoi(optarg);
				break;
			case 'd': // set destination vertex
				options.dest = atoi(optarg);
				break;
			case 'f': // set filename
				options.filename = optarg;
				break;
			default:
				break;
		}
	}

	// validation and printing error / usage messages

	bool valid = true;
		
	// check part validity
	if(options.part < PRINT_DFS || options.part > SHORTEST_PATH){
		fprintf(stderr,
			"please specify which part to run, using the -p flag:\n");
		fprintf(stderr, " -p 1: depth-first traversal (part 1)\n");
		fprintf(stderr, " -p 2: breadth-first traversal (part 2)\n");
		fprintf(stderr, " -p 3: finding a detailed path (part 3)\n");
		fprintf(stderr, " -p 4: finding all paths (part 4)\n");
		fprintf(stderr, " -p 5: finding the shortest path (part 5)\n");
		valid = false;
	}

	// validate vertex ids (as best we can right now, dont know # vertices)
	if(options.part >= PRINT_DFS && options.part <= SHORTEST_PATH) {
		if(options.source < 0){
			fprintf(stderr,
				"please provide a valid source index via the -s flag\n");
			valid = false;
		}
	}
	if(options.part >= DETAILED_PATH && options.part <= SHORTEST_PATH) {
		if(options.dest < 0) {
			fprintf(stderr,
				"please provide a valid destination index via the -d flag\n");
			valid = false;
		}
	}

	// check filename validity
	if(!options.filename){
		fprintf(stderr,
			"please provide a valid input filename via the -f flag\n");
		valid = false;
	}

	// check overall validity before continuing
	if(!valid){
		exit(EXIT_FAILURE);
	}

	return options;
}

// read data inside filename into a new graph
// exits on memory allocation failure or file format error
//
// format specification:
// NUMBER VERTICES (N)
// LIST OF N VERTEX LABELS (one per line, max line length MAX_LINE_LENGTH - 2)
// LIST OF EDGES (three ints per line: vertex_1 vertex_2 weight)
// END OF FILE
Graph* read_graph(char* filename) {
	
	// hook up the file
	FILE *file = fopen(filename, "r");
	if(!file){
		perror("error opening file");
		exit(EXIT_FAILURE);
	}

	// create graph with space for enough vertices
	int order = 0;
	if (!fscanf(file, "%d\n", &order)) {
		fprintf(stderr,"file format error: missing number of vertices\n");
		exit(EXIT_FAILURE);
	}
	Graph* graph = new_graph(order);

	// then, for each vertex label in file, create a new vertex
	int i;
	for(i = 0; i < order; i++) {
		char label[MAX_LINE_LEN];
		if (!fgets(label, MAX_LINE_LEN, file)) {
			fprintf(stderr,"file format error: not enough vertex labels\n");
			exit(EXIT_FAILURE);
		}
		label[strlen(label)-1] = '\0'; // strip newline

		graph_add_vertex(graph, label);
	}

	// next up, read in the edges and add them to the graph
	int u = 0, v = 0, w = 0;
	while (fscanf(file, "%d %d %d\n", &u, &v, &w) == 3) {
		graph_add_u_edge(graph, u, v, w);
	}

	// all done!
	fclose(file);
	return graph;
}

// checks vertex id options in use against largest graph vertex index
// exits with error messages if indices are too high
void validate_vertex_ids(Options options, int graph_size) {
	bool valid = true;

	// for all parts: validate source vertex
	if(options.part >= PRINT_DFS && options.part <= SHORTEST_PATH) {
		if(options.source >= graph_size){
			fprintf(stderr, "source index too high\n");
			valid = false;
		}
	}

	// for parts 3-5: validate destination vertex
	if(options.part >= DETAILED_PATH && options.part <= SHORTEST_PATH) {
		if(options.dest >= graph_size) {
			fprintf(stderr, "destination index too high\n");
			valid = false;
		}
	}

	if (!valid) {
		exit(EXIT_FAILURE);
	}
}