#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "graph.h"

// check if an array contains a value
bool array_contains(int *array, int count, int value)
{
	int i = 0;
	for (i = 0; i < count; i++)
		if (array[i] == value)
			return true;
	
	return false;
}


// determine is a vertex is constrained to only a single given color
bool vertex_must_be(struct vertex *vertex, int color)
{
	if (vertex->num_possible == 1 && vertex->current_value == color)
		return true;

	return false;
}


// determine whether a vertex's edges would disallow a given color to be chosen
bool edges_disallow_color(struct vertex *vertex, int color)
{
	bool in_edge = false;
	struct edge *last_edge = vertex->edges;

	while (last_edge != NULL) {
		if (last_edge->connects_to != NULL) {
			struct vertex *connected = last_edge->connects_to;
			in_edge = vertex_must_be(connected, color);
		}

		if (in_edge == true)
			return true;

		last_edge = last_edge->next;
	}

	return in_edge;
}


// See if a color is valid for a cell, not contained in any neighbors and not been removed
int is_valid_color(struct vertex *vertex, int *removed, int removed_count, int color)
{
	if (vertex->num_possible == 1 && vertex->current_value == color)
		return true;
	else
		if (!array_contains(removed, removed_count, color))
			return !edges_disallow_color(vertex, color);
	
	return false;
}


// Recursive backtracking algorithm
bool color_graph(struct vertex *vertex, int num_colors)
{
	if (vertex == NULL)
		return true;

	int removed_index = 0;
	int removed_colors[num_colors];
	
	int original_value = vertex->current_value;
	int original_num = vertex->num_possible;

	if (original_num == 1)
		return color_graph(vertex->next, num_colors);	

	int i = 0;
	for (i = 1; i <= original_num; i++) {
		if (!is_valid_color(vertex, removed_colors, removed_index, i))
			continue;

		vertex->current_value = i;
		vertex->num_possible = 1;

		bool success = color_graph(vertex->next, num_colors);

		if (success == false) {
			removed_colors[removed_index] = i;
			removed_index++;
			vertex->num_possible = original_num;
			vertex->current_value = original_value;
		}
		else {
			return true;
		}
	}

	return false;
}


// check if each of the vertices has only a single color possibility, meaning it's colored
bool graph_colored(struct graph *graph)
{
	struct vertex *next_vertex = graph->vertices;
	while (next_vertex != NULL) {
		if (next_vertex->num_possible > 1)
			return false;

		next_vertex = next_vertex->next;
	}

	return true;
}

