#include <stdbool.h>

struct graph {
	struct vertex *vertices;
	int vertex_index;
};


struct vertex {
	int current_value;
	int num_possible;
	int index;

	struct edge *edges;
	struct vertex *next;
};


struct edge {
	struct vertex *connects_to;
	struct edge *next;
};


struct graph *create_graph();
void free_graph(struct graph *graph);

struct vertex *add_vertex(struct graph *graph, int num_possible, int value);
void add_edge(struct vertex *vertex1, struct vertex *vertex2);

void print_graph(struct graph *graph);
void print_sudoku(struct graph *graph, int size);

bool color_graph(struct vertex *vertex, int num_colors);
bool graph_colored(struct graph *graph);
