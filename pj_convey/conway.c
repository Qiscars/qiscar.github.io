#include "conway.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// 构造新格点，分配内存
// malloc()
int food_count = 0;
Conway new_conway(const uint16_t m, const uint16_t n) {
	Conway c;
	c.m = m;
	c.n = n;
	c._grids = (int **)malloc(m * sizeof(GridState *));
	for (int i = 0; i < m; i++) {
		c._grids[i] = (int *)malloc(n * sizeof(GridState));
	}
	for (uint16_t i = 0; i < m; ++i) {
		for (uint16_t j = 0; j < n; ++j) {
			c._grids[i][j] = STATE_DEAD;	// 初始记为死
		}
	}
	return c;
}
// 删除格点，回收分配给格点二维数组的内存
// Conway游戏本身的结构体 c 不用删除，只删除其格点
// 使用free()
void delete_grids(Conway *c) {
	for (int i = 0; i < c->m; ++i) {
		free(c->_grids[i]);
	}
	free(c->_grids);
}

// 随机地初始化格点
/*void init_random(Conway *c) {
	srand(time(NULL));
	for (int i = 0; i < c->m; i++) {
		for (int j = 0; j < c->n; j++) {
			c->_grids[i][j] = rand() % 2;	 // 随机生死
		}
	}
}*/
void init_random(Conway *c) {
	food_count = 0;
	srand(time(NULL));
	for (int i = 0; i < c->m; i++) {
		for (int j = 0; j < c->n; j++) {
			c->_grids[i][j] = rand() % 2;	 // 随机生死
		}
	}
	for (int i = food_count; i < (c->m + 1) / 2; ++i) {
		int food_x = rand() % c->m;
		int food_y = rand() % c->n;
		// Ensure one food resource per location
		if (c->_grids[food_x][food_y] != STATE_FOOD) {
			c->_grids[food_x][food_y] = STATE_FOOD;
		} else {
			--i;	// 位置不符合就退回重新
		}
	}
	food_count = (c->m + 1) / 2;
}
int count_alive_neighbors(const Conway *c, int x, int y) {
	int alive = 0;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (i == 0 && j == 0) continue;	 // Skip the cell itself
			int nx = x + i;
			int ny = y + j;
			if (nx >= 0 && nx < c->m && ny >= 0 && ny < c->n) {
				alive += c->_grids[nx][ny] == STATE_ALIVE ? 1 : 0;
			}
		}
	}
	return alive;
}
// 将系统演化到下一世代
/*void next_generation(Conway *c) {
	GridState **new_grid = (int **)malloc(c->m * sizeof(GridState *));
	for (int i = 0; i < c->m; i++) {
		new_grid[i] = (int *)malloc(c->n * sizeof(GridState));
		for (int j = 0; j < c->n; j++) {
			int alive_neighbors = count_alive_neighbors(c, i, j);
			if (c->_grids[i][j] == STATE_ALIVE &&
					(alive_neighbors < 2 || alive_neighbors > 3)) {
				new_grid[i][j] = STATE_DEAD;
			} else if (c->_grids[i][j] == STATE_DEAD && alive_neighbors == 3) {
				new_grid[i][j] = STATE_ALIVE;
			} else {
				new_grid[i][j] = c->_grids[i][j];
			}
		}
	}
	delete_grids(c);
	c->_grids = new_grid;
	new_grid = NULL;
}*/
void next_generation(Conway *c) {
	int foodconsume_Flag = 0;
	GridState **new_grid = (int **)malloc(c->m * sizeof(GridState *));
	for (int i = 0; i < c->m; i++) {
		new_grid[i] = (int *)malloc(c->n * sizeof(GridState));
	}
	while (food_count < (c->m + 1) / 2) {
		int food_x = rand() % c->m;
		int food_y = rand() % c->n;
		// Ensure one food resource per location
		if (c->_grids[food_x][food_y] != STATE_FOOD &&
				c->_grids[food_x][food_y] != Selected_ALIVE) {
			if (c->_grids[food_x][food_y] == STATE_ALIVE) {
				c->_grids[food_x][food_y] = Selected_ALIVE;
				new_grid[food_x][food_y] = STATE_ALIVE_WITH_FOOD;
				foodconsume_Flag++;
				food_count++;
			} else {
				c->_grids[food_x][food_y] = STATE_FOOD;
				food_count++;
			}
		}
	}
	for (int i = 0; i < c->m; i++) {
		// new_grid[i] = (int *)malloc(c->n * sizeof(GridState));
		for (int j = 0; j < c->n; j++) {
			int alive_neighbors = count_alive_neighbors(c, i, j);
			if (c->_grids[i][j] == STATE_ALIVE &&
					(alive_neighbors < 2 || alive_neighbors > 3)) {
				new_grid[i][j] = STATE_DEAD;
			} else if (c->_grids[i][j] == STATE_DEAD && alive_neighbors == 3) {
				new_grid[i][j] = STATE_ALIVE;
			} else if (c->_grids[i][j] == STATE_ALIVE_WITH_FOOD) {
				new_grid[i][j] = STATE_ALIVE;
			} else if (c->_grids[i][j] == Selected_ALIVE) {
				continue;
			} else if (c->_grids[i][j] == STATE_ALIVE &&
								 (alive_neighbors == 2 || alive_neighbors == 3)) {
				new_grid[i][j] = STATE_ALIVE;
			} else {
				new_grid[i][j] = c->_grids[i][j];
			}
			// Handle food consumption and movement
			// Implement rules for food consumption and movement
			if (c->_grids[i][j] == STATE_FOOD) {
				// Check neighboring cells for alive cells
				int alive_cells = 0;
				int alive_cell_i = -1, alive_cell_j = -1;
				for (int di = -1; di <= 1; di++) {
					for (int dj = -1; dj <= 1; dj++) {
						if (di == 0 && dj == 0) continue;	 // Skip the food cell itself
						int ni = i + di, nj = j + dj;
						if (ni >= 0 && ni < c->m && nj >= 0 && nj < c->n &&
								c->_grids[ni][nj] == STATE_ALIVE) {
							alive_cells++;
							alive_cell_i = ni;
							alive_cell_j = nj;
						}
					}
				}
				if (alive_cells == 1) {
					// If exactly one alive cell is neighboring, it consumes the
					// food
					c->_grids[alive_cell_i][alive_cell_j] = Selected_ALIVE;
					new_grid[alive_cell_i][alive_cell_j] = STATE_DEAD;
					new_grid[i][j] = STATE_ALIVE_WITH_FOOD;	 // Remove the food resource
					foodconsume_Flag++;
					c->_grids[i][j] = CONSUME_FOOD;
				} else if (alive_cells > 1) {
					// If more than one alive cell is neighboring, randomly select
					// one to consume the food
					int selected = rand() % alive_cells;
					int count = 0;
					for (int di = -1; di <= 1; di++) {
						for (int dj = -1; dj <= 1; dj++) {
							if (di == 0 && dj == 0) continue;	 // Skip the food cell itself
							int ni = i + di, nj = j + dj;
							if (ni >= 0 && ni < c->m && nj >= 0 && nj < c->n &&
									c->_grids[ni][nj] == STATE_ALIVE) {
								if (count == selected) {
									c->_grids[i][j] = CONSUME_FOOD;
									c->_grids[ni][nj] = Selected_ALIVE;
									new_grid[ni][nj] = STATE_DEAD;
									new_grid[i][j] =
											STATE_ALIVE_WITH_FOOD;	// Remove the food resource
									foodconsume_Flag++;
									break;
								} else
									count++;
							}
						}
						if (count == selected) break;
					}
				}
			}
		}
	}
	food_count = food_count - foodconsume_Flag;
	foodconsume_Flag = 0;
	delete_grids(c);
	c->_grids = new_grid;
	new_grid = NULL;
}
// Get the state of a specific cell
// 获取格点的当前状态
// 注意下标边界检查
// 0 <= x < m,
// 0 <= y < n,
// 虽然看上去这样一个封装没有必要，毕竟可以 c->_grids[x][y]来访问
// 但是封装后会安全一点
// 越界或者遇到空指针返回GridState::None ?
// if (get_current_state(c, x, y) == GridState::None) {
//     // balabalabala
// }
GridState get_state(const Conway *c, const uint16_t x, const uint16_t y) {
	if (x >= c->m || x < 0 || y >= c->n || y < 0) {
		return STATE_NONE;
	}
	return c->_grids[x][y];
}

void set_state(Conway *c, const uint16_t x, const uint16_t y, GridState s) {
	if (x < c->m && y < c->n) {
		c->_grids[x][y] = s;
	}
}

// 获取格点下一个世代的状态
// 注意下标边界检查
// 0 <= x < m,
// 0 <= y < n,
GridState get_next_state(const Conway *c, const uint16_t x, const uint16_t y) {
	if (x >= c->m || y >= c->n || x < 0 || y < 0) {
		return STATE_NONE;
	}

	int alive_neighbors = count_alive_neighbors(c, x, y);
	GridState current_state = c->_grids[x][y];

	// Apply Conway's Game of Life rules
	if (current_state == STATE_ALIVE &&
			(alive_neighbors < 2 || alive_neighbors > 3)) {
		return STATE_DEAD;
	} else if (current_state == STATE_DEAD && alive_neighbors == 3) {
		return STATE_ALIVE;
	} else {
		return current_state;
	}
}

// 展示格点，一般来说是printf打印吧
// 不过长和宽设置的都是uint16_t类型，稍微大一点的格点就不好打印了
void show_conway(const Conway *c) {
	for (int i = 0; i < c->m; i++) {
		for (int j = 0; j < c->n; j++) {
			char cell_char = '0';
			switch (c->_grids[i][j]) {
				case STATE_ALIVE:
					cell_char = '*';
					break;
				case Selected_ALIVE:
					cell_char = '*';
					break;
				case STATE_DEAD:
					cell_char = ' ';
					break;
				case STATE_FOOD:
					cell_char = 'F';	// Representing food
					break;
				case STATE_ALIVE_WITH_FOOD:
					cell_char = 'S';
					break;
			}
			printf("|%c", cell_char);
		}
		printf("|\n");
	}
}

// 保存格点到文件（可能得考虑一下数据保存到文件的格式）
// 成功则返回0，失败返回非0值
int save_conway(const Conway *c, const char *path) {
	FILE *file = fopen(path, "w");
	if (!file) {
		return -1;	// Error opening file
	}

	fprintf(file, "%d,%d\n", c->m, c->n);
	for (int i = 0; i < c->m; i++) {
		for (int j = 0; j < c->n; j++) {
			fprintf(file, "%d,", c->_grids[i][j]);
		}
		fprintf(file, "\n");
	}

	fclose(file);
	return 0;	 // if succeed, retun 0
}

// 从文件读取格点
// 失败则Conway._grids = NULL
// 涉及malloc()
Conway new_conway_from_file(const char *path) {
	FILE *file = fopen(path, "r");
	if (!file) {
		Conway c = {0, 0, NULL};
		printf("Error: File could not be opened.\n");
		return c;	 // Error opening file, return empty grid
	}

	uint16_t m, n;
	if (fscanf(file, "%hu,%hu", &m, &n) != 2) {
		fclose(file);
		Conway c = {0, 0, NULL};
		printf("Error: Invalid grid dimensions in file.\n");
		return c;	 // Invalid format for dimensions
	}

	// Check for sensible dimensions
	if (m == 0 || n == 0) {
		fclose(file);
		Conway c = {0, 0, NULL};
		printf("Error: Grid dimensions are zero.\n");
		return c;	 // Invalid grid size
	}

	Conway c = new_conway(m, n);	// Initialize Conway struct
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			int state;
			if (fscanf(file, "%d,", &state) != 1 ||
					(state != STATE_DEAD && state != STATE_ALIVE)) {
				fclose(file);
				delete_grids(&c);
				Conway c = {0, 0, NULL};
				printf("Error: Invalid cell state in file.\n");
				return c;	 // Invalid cell state
			}
			c._grids[i][j] = state;
		}
	}

	fclose(file);
	return c;	 // Return the initialized grid
}
