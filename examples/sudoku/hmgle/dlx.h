#ifndef _DLX_H
#define _DLX_H

#include <stdint.h>
#include "debug_print.h"

struct dlx_col;
struct dlx_node;

struct dlx_node {
	struct dlx_node *lx;
	struct dlx_node *rx;
	struct dlx_node *ux;
	struct dlx_node *dx;
	struct dlx_col *colx;
	int row_id;
};

struct dlx_col {
	struct dlx_node c;
	int s;
	int id;
};

struct dlx_head {
	int col_num;
	int row_num;
	int size_node;
	struct dlx_node h;
	struct dlx_col *c;
};

struct location {
	int r;
	int c;
};

struct dlx_matrix {
	int col_num;
	int row_num;
	int size;
	struct location *bitset;
};

void dlx_header_init(struct dlx_head *h, int col_num, int row_num);
void dlx_header_release(struct dlx_head *h);
void dlx_add_node_to_header(struct dlx_head *h, const struct location *loc);
struct dlx_matrix *alloc_matrix_via_str(struct dlx_matrix *matrix,
				const char *str, int col_num, int row_num);
void free_matrix(struct dlx_matrix *matrix);
void matrix_to_header(struct dlx_head *h, const struct dlx_matrix *matrix);
struct dlx_col *min_s_col(const struct dlx_head *h);
void dlx_cover_col(struct dlx_col *col);
void dlx_uncover_col(struct dlx_col *col);
int dlx_select_row(struct dlx_node *node);
void dlx_unselect_row(struct dlx_node *node);
int dlx_search(struct dlx_head *h, int *solution, int sel_row_num, int *is_run);

#endif
