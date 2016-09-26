#include "dlx_sudoku.h"

int select_col0_by_row(int row)
{
	return row / SUDOKU_RANK;
}

int select_col1_by_row(int row)
{
	return SUDOKU_RANK * SUDOKU_RANK
		+ row / (SUDOKU_RANK * SUDOKU_RANK) * SUDOKU_RANK
		+ row % SUDOKU_RANK;
}

int select_col2_by_row(int row)
{
	return SUDOKU_RANK * SUDOKU_RANK * 2
		+ row % (SUDOKU_RANK * SUDOKU_RANK);
}

int select_col3_by_row(int row)
{
	return SUDOKU_RANK * SUDOKU_RANK * 3
		+ (row / (SUDOKU_RANK * SUDOKU_RANK * 3)) * SUDOKU_RANK * 3
		+ (row % (SUDOKU_RANK * SUDOKU_RANK * 3))
		% (SUDOKU_RANK * SUDOKU_RANK) / (SUDOKU_RANK * 3)
		* SUDOKU_RANK
		+ row % SUDOKU_RANK;
}

struct dlx_node *find_row_node(struct dlx_head *h, int row)
{
	struct dlx_node *node;
	struct dlx_node *col;
	int col_id;

	col_id = select_col0_by_row(row);
	for (col = h->h.rx; col != &h->h; col = col->rx) {
		if (col->colx->id == col_id) {
			for (node = col->dx; node != col; node = node->dx) {
				if (node->row_id == row) {
					return node;
				}
			}
		}
	}
	return NULL;
}

struct sudoku_dsr * str2sudoku(struct sudoku_dsr *sudoku, int w, int h,
				char *str, size_t str_lenth)
{
	size_t i;
	int n = 0;

	assert(sudoku != NULL && w > 0 && h > 0);
	sudoku->w = w;
	sudoku->h = h;
	for (i = 0; i < str_lenth; i++) {
		if (str[i] >= '1' && str[i] <= '9') {
			*(sudoku->data + n) = str[i] - '0';
		} else if (str[i] != '*'
			   && str[i] != '.'
			   && str[i] != '0'
			   && str[i] != 'x'
			   && str[i] != 'X') {
			continue;
		} else {
			*(sudoku->data + n) = 0;
		}
		if (++n > w * h) {
			break;
		}
	}
	return sudoku;
}

int cell2row(int r, int c, int val)
{
	assert(val > 0 && val < 10 && r >= 0 && r < 9 && c >= 0 && c < 9);
	return r * SUDOKU_RANK * SUDOKU_RANK + c * SUDOKU_RANK + val - 1;
}

struct sudoku_cell *row2cell(int row, struct sudoku_cell *cell)
{
	cell->r = row / (SUDOKU_RANK * SUDOKU_RANK);
	cell->c = (row / SUDOKU_RANK) % SUDOKU_RANK;
	cell->val = row % SUDOKU_RANK + 1;
	return cell;
}

void set_sudoku_cell(struct sudoku_dsr *sudoku, const struct sudoku_cell *cell)
{
	int r, c;

	r = cell->r;
	c = cell->c;
	sudoku->data[SUDOKU_RANK * r + c] = cell->val;
}

void set_sudoku_cell_via_row(struct sudoku_dsr *sudoku, int row)
{
	struct sudoku_cell cell;

	row2cell(row, &cell);
	set_sudoku_cell(sudoku, &cell);
}

int set_dlx_h_sudoku(struct dlx_head *h, const struct sudoku_dsr *sudoku,
			struct dlx_node **save_node)
{
	int i, j;
	int val;
	int row;
	int n = 0;

	assert(h && sudoku);
	for (i = 0; i < sudoku->h; i++) {
		for (j = 0; j < sudoku->w; j++) {
			val = *(sudoku->data + i * sudoku->w + j);
			if (val > 0) {
				row = cell2row(i, j, val);
				*(save_node + n) = find_row_node(h, row);
				if (dlx_select_row(*(save_node + n)) < 0) {
					return -1;
				}
				n++;
			}
		}
	}
	return 0;
}

void print_sudoku_str(const struct sudoku_dsr *sudoku)
{
	int i, j;

	for (i = 0; i < SUDOKU_RANK; i++) {
		for (j = 0; j < SUDOKU_RANK; j++) {
			printf("%d", *(sudoku->data + i * SUDOKU_RANK + j));
		}
	}
	printf("\n");
}

void print_sudoku(const struct sudoku_dsr *sudoku)
{
	int i, j;

	for (i = 0; i < SUDOKU_RANK; i++) {
		if (i % 3 == 0) {
			printf("+-------+-------+-------+\n");
		}
		for (j = 0; j < SUDOKU_RANK; j++) {
			if (j % 3 == 0) {
				printf("| ");
			}
			printf("%d ", *(sudoku->data + i * SUDOKU_RANK + j));
			if (j == SUDOKU_RANK - 1) {
				printf("|");
			}
		}
		printf("\n");
	}
	printf("+-------+-------+-------+\n");
}
