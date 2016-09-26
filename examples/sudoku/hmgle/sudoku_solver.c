#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "dlx.h"
#include "dlx_sudoku.h"
#include "debug_print.h"

static void show_usage(const char *pro_name)
{
	fprintf(stderr, "Usage: %s [Options] [inputfile]\n"
			"\n"
			"Options:\n"
			"        -v show more info\n"
			"        -h show this help\n"
			"\n", pro_name);
}

static void init_sudoku_dlx_h(struct dlx_head *h)
{
	struct dlx_matrix matrix;
	char input[MAX_COLUMN * MAX_ROW + 1];
	size_t i;

	memset(&input, 0, sizeof(input));
	memset(&matrix, 0, sizeof(struct dlx_matrix));
	for (i = 0; i < MAX_COLUMN * MAX_ROW; i++) {
		input[i] = '0';
	}
	for (i = 0; i < MAX_ROW; i++) {
		int loc;
		loc = i * MAX_COLUMN + select_col0_by_row(i);
		input[loc] = '1';
		loc = i * MAX_COLUMN + select_col1_by_row(i);
		input[loc] = '1';
		loc = i * MAX_COLUMN + select_col2_by_row(i);
		input[loc] = '1';
		loc = i * MAX_COLUMN + select_col3_by_row(i);
		input[loc] = '1';
	}

	alloc_matrix_via_str(&matrix, input, MAX_COLUMN, MAX_ROW);
	matrix_to_header(h, &matrix);
	free_matrix(&matrix);
}

static void read_sudoku(FILE *in, char *read_input)
{
	char ch;
	int n = 0;

	while (n < 81 && (ch = fgetc(in)) != EOF) {
		if ((ch >= '0' && ch <= '9') ||
		    ch == '*' || ch == '.' ||
		    ch == 'x' || ch == 'X') {
			read_input[n++] = ch;
		}
	}
}

int main(int argc, char **argv)
{
	size_t i;
	struct dlx_head dlx_h;
	int is_run = 1;
	int n;
	int *solution;
	struct dlx_node *sel_row[MAX_ROW];
	char read_input[256] = {0};
	struct sudoku_dsr sudoku;
	int opt;
	int show_sudoku_flag = 0;
	FILE *in = stdin;

	while ((opt = getopt(argc, argv, "vh?")) != -1) {
		switch (opt) {
		case 'v':
			show_sudoku_flag = 1;
			break;
		case 'h':
		default:
			show_usage(argv[0]);
			exit(1);
		}
	}
	if (argc > optind) {
		in = fopen(argv[optind], "r");
		if (!in) {
			fprintf(stderr,
				"Unable to open input file \"%s\": %s\n",
				argv[optind], strerror(errno));
			return 1;
		}
	}

	memset(sel_row, 0, sizeof(sel_row));

	solution = malloc(sizeof(int) * MAX_ROW);
	memset(solution, 0, sizeof(int) * MAX_ROW);
	memset(&dlx_h, 0, sizeof(struct dlx_head));
	dlx_header_init(&dlx_h, MAX_COLUMN, MAX_ROW);
	init_sudoku_dlx_h(&dlx_h);

	sudoku.data = malloc(sizeof(*sudoku.data) * SUDOKU_RANK	* SUDOKU_RANK);
	memset(sudoku.data, 0, sizeof(*sudoku.data) * SUDOKU_RANK * SUDOKU_RANK);

	read_sudoku(in, read_input);
	str2sudoku(&sudoku, SUDOKU_RANK, SUDOKU_RANK,
			read_input, sizeof(read_input));
	if (set_dlx_h_sudoku(&dlx_h, &sudoku, sel_row) < 0) {
		/* invalid sudoku input */
		fprintf(stderr, "Invalid sudoku, no solution!\n");
		exit(1);
	}
	if (show_sudoku_flag) {
		printf("input sudoku:\n");
		print_sudoku(&sudoku);
	}

	n = dlx_search(&dlx_h, solution, 0, &is_run);
	if (show_sudoku_flag) {
		printf("dlx_search return %d:\n", n);
	}
	if (n > 0) {
		for (i = 0; i < (size_t)n; i++) {
			set_sudoku_cell_via_row(&sudoku, solution[i]);
		}
	}
	if (show_sudoku_flag) {
		print_sudoku(&sudoku);
	} else {
		print_sudoku_str(&sudoku);
	}

	/* for free */
	for (i = 0; i < sizeof(sel_row) / sizeof(sel_row[0]); i++) {
		if (sel_row[i]) {
			dlx_unselect_row(sel_row[i]);
		}
	}

	if (in != stdin)
		fclose(in);
	free(sudoku.data);
	dlx_header_release(&dlx_h);
	free(solution);
	return 0;
}
