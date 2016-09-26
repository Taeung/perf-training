#include <stdlib.h>
#include "dlx.h"

void dlx_header_init(struct dlx_head *h, int col_num, int row_num)
{
	int i;

	h->col_num = col_num;
	h->row_num = row_num;
	h->size_node = 0;
	h->c = malloc(sizeof(struct dlx_col) * col_num);
	memset(h->c, 0, sizeof(sizeof(struct dlx_col) * col_num));
	memset(&h->h, 0, sizeof(struct dlx_node));

	h->h.rx = &h->c[0].c;
	for (i = 0; i < col_num - 1; i++) {
		h->c[i].c.rx = &h->c[i + 1].c;
	}
	h->c[col_num - 1].c.rx = &h->h;

	h->h.lx = &h->c[col_num - 1].c;
	for (i = col_num - 1; i > 0; i--) {
		h->c[i].c.lx = &h->c[i - 1].c;
	}
	h->c[0].c.lx = &h->h;

	for (i = 0; i < col_num; i++) {
		h->c[i].c.dx = &h->c[i].c;
		h->c[i].c.ux = &h->c[i].c;
		h->c[i].c.colx = &h->c[i];
		h->c[i].s = 0;
		h->c[i].id = i;
	}
}

void dlx_header_release(struct dlx_head *h)
{
	struct dlx_node *p;

	for (p = h->h.rx; p != &h->h; p = p->rx) {
		struct dlx_node *c, *n;
		for (c = p->dx; c != p; ) {
			n = c->dx;
			free(c);
			c = n;
		}
	}
	free(h->c);
}

static void insert_up_node(struct dlx_node *new, struct dlx_node *node)
{
	assert(new != NULL && node != NULL);
	new->ux = node->ux;
	new->dx = node;
	new->rx = new->lx = new;
	new->colx = node->colx;
	node->colx->s += 1;
	node->ux->dx = new;
	node->ux = new;
}

static int dlx_col_add_node(struct dlx_col *col, struct dlx_node *cell)
{
	struct dlx_node *p;

	for (p = col->c.dx; p != &col->c; p = p->dx) {
		if (p->row_id == cell->row_id) {
			return -1;
		}
		if (p->row_id > cell->row_id) {
			insert_up_node(cell, p);
			return 0;
		}
	}
	insert_up_node(cell, &col->c);
	return 0;
}

static int dlx_row_add_node(struct dlx_head *h, struct dlx_node *cell)
{
	struct dlx_node *col;
	struct dlx_node *p;

	for (col = h->h.rx; col != &h->h; col = col->rx) {
		for (p = col->dx; p != col; p = p->dx) {
			if (p->row_id == cell->row_id) {
				if (p->colx->id > cell->colx->id) {
					cell->rx = p;
					cell->lx = p->lx;
					p->lx->rx = cell;
					p->lx = cell;
					return 0;
				}
				struct dlx_node *curr;
				for (curr = p->rx; curr != p; curr = curr->rx) {
					if (curr->colx->id > cell->colx->id) {
						cell->rx = curr;
						cell->lx = curr->lx;
						curr->lx->rx = cell;
						curr->lx = cell;
						return 0;
					}
				}
				curr = curr->lx;
				cell->lx = curr;
				cell->rx = curr->rx;
				curr->rx->lx = cell;
				curr->rx = cell;
				return 0;
			}
		}
	}
	return 0;
}

void dlx_add_node_to_header(struct dlx_head *h, const struct location *loc)
{
	struct dlx_node *cell;
	int ret;

	assert(h);
	cell = malloc(sizeof(struct dlx_node));
	memset(cell, 0, sizeof(struct dlx_node));
	cell->row_id = loc->r;
	ret = dlx_col_add_node(&h->c[loc->c], cell);
	if (ret < 0) {
		free(cell);
		assert(0);
	}
	ret = dlx_row_add_node(h, cell);
	if (ret < 0) {
		free(cell);
		assert(0);
	}
}

static int how_many_1_in_str(const char *str)
{
	int ret = 0;
	int len = strlen(str);
	int i;

	for (i = 0; i < len; i++)
		if (str[i] == '1')
			ret++;
	return ret;
}

struct dlx_matrix *alloc_matrix_via_str(struct dlx_matrix *matrix,
				const char *str, int col_num, int row_num)
{
	int i;
	int len = strlen(str);
	int index = 0;

	matrix->col_num = col_num;
	matrix->row_num = row_num;
	matrix->size = how_many_1_in_str(str);
	matrix->bitset = malloc(sizeof(struct location) * matrix->size);
	for (i = 0; i < len; i++) {
		if (str[i] == '1') {
			matrix->bitset[index].r = i / col_num;
			matrix->bitset[index++].c = i % col_num;
		}
	}
	return matrix;
}

void free_matrix(struct dlx_matrix *matrix)
{
	if (matrix->bitset) {
		free(matrix->bitset);
		matrix->bitset = NULL;
	}
}

void matrix_to_header(struct dlx_head *h, const struct dlx_matrix *matrix)
{
	int i;

	for (i = 0; i < matrix->size; i++) {
		dlx_add_node_to_header(h, matrix->bitset + i);
	}
}

struct dlx_col *min_s_col(const struct dlx_head *h)
{
	struct dlx_col *min_col;
	struct dlx_node *n;

	assert(h->h.rx != &h->h);
	min_col = h->h.rx->colx;
	for (n = h->h.rx->rx; n != &h->h; n = n->rx) {
		if (n->colx->s < min_col->s) {
			min_col = n->colx;
		}
	}
	return min_col;
}

static void strike_node_lr(struct dlx_node *col)
{
	col->lx->rx = col->rx;
	col->rx->lx = col->lx;
}

static void strike_node_ud(struct dlx_node *col)
{
	col->ux->dx = col->dx;
	col->dx->ux = col->ux;
}

static void unstrike_node_lr(struct dlx_node *col)
{
	col->lx->rx = col;
	col->rx->lx = col;
}

static void unstrike_node_ud(struct dlx_node *col)
{
	col->ux->dx = col;
	col->dx->ux = col;
}

void dlx_cover_col(struct dlx_col *col)
{
	struct dlx_node *m, *n;

	strike_node_lr(&col->c);
	for (m = col->c.dx; m != &col->c; m = m->dx) {
		for (n = m->rx; n != m; n = n->rx) {
			strike_node_ud(n);
			n->colx->s -= 1;
		}
	}
}

void dlx_uncover_col(struct dlx_col *col)
{
	struct dlx_node *m, *n;

	for (m = col->c.ux; m != &col->c; m = m->ux) {
		for (n = m->lx; n != m; n = n->lx) {
			unstrike_node_ud(n);
			n->colx->s += 1;
		}
	}
	unstrike_node_lr(&col->c);
}

int dlx_select_row(struct dlx_node *node)
{
	struct dlx_node *t = node;

	if (node == NULL) {
		return -1;
	}
	do {
		dlx_cover_col(t->colx);
	} while ((t = t->rx) != node);
	return 0;
}

void dlx_unselect_row(struct dlx_node *node)
{
	struct dlx_node *t = node;

	do {
		dlx_uncover_col(t->colx);
	} while ((t = t->lx) != node);
}

int dlx_search(struct dlx_head *h, int *solution, int sel_row_num, int *is_run)
{
	int n = 0;	/* 0: no solution */
	struct dlx_col *min_col;
	struct dlx_node *node;
	struct dlx_node *m;

	if (h->h.rx == &h->h) {
		*is_run -= 1;
		return sel_row_num;
	}
	min_col = min_s_col(h);
#if 0
	if (min_col->s < 1) {
		return 0;
	}
	debug_print("min_col s is %d, id is %d", min_col->s, min_col->id);
#endif
	dlx_cover_col(min_col);
	for (node = min_col->c.dx; node != &min_col->c; node = node->dx) {
		for (m = node->rx; m != node; m = m->rx) {
			dlx_cover_col(m->colx);
		}
		n = dlx_search(h, solution, sel_row_num + 1, is_run);
		for (m = node->lx; m != node; m = m->lx) {
			dlx_uncover_col(m->colx);
		}
		if (n > 0) {
			solution[sel_row_num] = node->row_id;
		}
		if (*is_run == 0) {
			break;
		}
	}
	dlx_uncover_col(min_col);
	return n;
}
