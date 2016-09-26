#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>

#include "graph.h"

// first 3 3x3 groups, in a row
int initial_groups[3][9] = {
	{0,1,2, 9,10,11, 18,19,20},
	{27,28,29, 36,37,38, 45,46,47},
	{54,55,56, 63,64,65, 72,73,74}
};


// check if a given index is in a (usually) 9x9 group
bool index_in_group(int *group, int size, int index)
{
	int i = 0;
	for (i = 0; i < size; i++)
		if (group[i] == index)
			return true;
	
	return false;
}


// link a vertex at index with every other vertex in the group
void link_vertex_with_group(int *group, int size, int index, struct vertex **vertices)
{
	struct vertex *to_link = vertices[index];

	int i = 0;
	for (i = 0; i < size; i++)
		if (group[i] != index)
			add_edge(to_link, vertices[group[i]]);
}


// link 3x3 groups in the list of vertices
void link_three_groups(struct vertex **vertices)
{
	// link 3x3 groups from initial data
	int i = 0;
	int j = 0;
	int current_group[9];

	for (i = 0; i < 9; i++) {
		int factor = 0;
		if (i > 2 && i < 6)
			factor = 1;
		else if (i > 5)
			factor = 2;

		for (j = 0; j < 9; j++)
			current_group[j] = initial_groups[i % 3][j] + (factor * 3); 

		// search through vertex indices and link vertexes in a group together
		for (j = 0; j < 81; j++)
			if (index_in_group(current_group, 9, j))
				link_vertex_with_group(current_group, 9, j, vertices);

	}
}


// link rows and columns together
void link_cols_and_rows(struct vertex **vertices)
{
	int current_row[9];
	int current_col[9];
	int i = 0;
	int j = 0;
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			current_row[j] = (i * 9) + j;
			current_col[j] = (j * 9) + i;
		}

		// search through vertex indices and link vertexes in a group together
		for (j = 0; j < 81; j++) 
			if (index_in_group(current_row, 9, j))
				link_vertex_with_group(current_row, 9, j, vertices);

		for (j = 0; j < 81; j++) 
			if (index_in_group(current_col, 9, j))
				link_vertex_with_group(current_col, 9, j, vertices);
	}
}

		
// create graph, load up vertices and link them together in the sudoku pattern
struct graph *load_initial()
{
	// load sudoku puzzle in from stdin
	int puzzle_size = 9;
	int num_chars = (puzzle_size * puzzle_size) + 1;
	int num_boxes = puzzle_size * puzzle_size;

	char *buffer = malloc(sizeof(char) * num_chars);
	fgets(buffer, num_chars, stdin);

	struct graph *graph = create_graph();
	struct vertex **vertices = malloc(sizeof(void *) * num_boxes);

	// set up the graph with the vertices
	int i = 0;
	for (i = 0; i < num_boxes; i++) { 
		// add all 9 color possibilities if we get a 0, which is an unfilled cell
		if (buffer[i] == '0') {
			vertices[i] = add_vertex(graph, 9, 0);
		}
		else {
			// add a single possibility if we get a pre-filled cell
			vertices[i] = add_vertex(graph, 1, buffer[i] - '0');
		}
	}

	link_three_groups(vertices);
	link_cols_and_rows(vertices);

	free(buffer);
	free(vertices);

	return graph;
}


int main(int argc, char **argv)
{
	struct graph *graph = load_initial();

	color_graph(graph->vertices, 9);

	if (graph_colored(graph))
		print_sudoku(graph, 9);
	else
		printf("Unable to color sudoku graph.\n");

	free_graph(graph);
	
	return 0;
}
