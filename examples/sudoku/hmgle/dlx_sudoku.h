#ifndef _DLX_SUDOKU_H
#define _DLX_SUDOKU_H

#include "dlx.h"

#define MAX_COLUMN	324	/* 81 cells + (9 + 9 + 9) * 9 */
#define MAX_ROW		729	/* 81 cells * 9  */
#define SUDOKU_RANK	9

struct sudoku_dsr {
	int w;
	int h;
	int *data;
};

struct sudoku_cell {
	int r;
	int c;
	int val;
};

int select_col0_by_row(int row);
int select_col1_by_row(int row);
int select_col2_by_row(int row);
int select_col3_by_row(int row);
struct dlx_node *find_row_node(struct dlx_head *h, int row);

struct sudoku_dsr *str2sudoku(struct sudoku_dsr *sudoku, int w, int h,
				char *str, size_t str_lenth);
int cell2row(int r, int c, int val);
struct sudoku_cell *row2cell(int row, struct sudoku_cell *cell);
void set_sudoku_cell(struct sudoku_dsr *sudoku, const struct sudoku_cell *cell);
void set_sudoku_cell_via_row(struct sudoku_dsr *sudoku, int row);
int set_dlx_h_sudoku(struct dlx_head *h, const struct sudoku_dsr *sudoku,
			struct dlx_node **save_node);
void print_sudoku_str(const struct sudoku_dsr *sudoku);
void print_sudoku(const struct sudoku_dsr *sudoku);

#endif
