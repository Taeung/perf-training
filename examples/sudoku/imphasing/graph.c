#include <stdlib.h>
#include <stdio.h>
#include "graph.h"

struct graph *create_graph()
{
	struct graph *created = malloc(sizeof(struct graph));
	created->vertices = NULL;
	created->vertex_index = 1;

	return created;
}


void free_graph(struct graph *graph)
{
	struct vertex *next_vertex = graph->vertices;

	while (next_vertex != NULL) {
		struct vertex *current = next_vertex;
		next_vertex = next_vertex->next;

		struct edge *next_edge = current->edges;
		while (next_edge != NULL) {
			struct edge *current_edge = next_edge;
			next_edge = next_edge->next;

			free(current_edge);
		}

		free(current);
	}

	free(graph);
}


struct vertex *add_vertex(struct graph *graph, int num_possible, int value)
{
	struct vertex *next = malloc(sizeof(struct vertex));
	next->index = graph->vertex_index++;
	next->current_value = value;
	next->num_possible = num_possible;
	next->edges = NULL;
	next->next = graph->vertices;
	graph->vertices = next;

	return next;
}


void add_edge(struct vertex *vertex1, struct vertex *vertex2)
{
	struct edge *next = malloc(sizeof(struct edge));
	next->connects_to = vertex2;
	next->next = vertex1->edges;
	vertex1->edges = next;
}


void print_graph(struct graph *graph)
{
	int edge_count = 0;
	struct vertex *last_vertex = graph->vertices;
	
	while (last_vertex != NULL) {
		printf("Vertex %d[%d] connects to vertices: ", last_vertex->index, last_vertex->current_value);
		struct edge *last_edge = last_vertex->edges;

		while (last_edge != NULL) {
			if (last_edge->connects_to != NULL) {
				edge_count++;
				printf("%d[%d], ", last_edge->connects_to->index,
					last_edge->connects_to->current_value);
			}

			last_edge = last_edge->next;
		}

		printf("\n");

		last_vertex = last_vertex->next;
	}

	printf("Total edge count: %d\n", edge_count);
}


void print_sudoku(struct graph *graph, int size)
{
	int *output = malloc(sizeof(int) * (size * size));

	struct vertex *last_vertex = graph->vertices;

	int index = 0;
	while (last_vertex != NULL) {
		output[index] = last_vertex->current_value;
		
		last_vertex = last_vertex->next;
		index++;
	}

	index--;
	for (; index >= 0; index--)
		printf("%d", output[index]);

	printf("\n");

	free(output);
}

