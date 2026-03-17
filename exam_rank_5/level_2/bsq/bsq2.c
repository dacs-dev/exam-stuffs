/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsq2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/17 06:26:48 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/17 06:27:17 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bsq.h"

void	print_map(t_map *m)
{
	for (int i = 0; i < m->rows; i++)
		fprintf(stdout, "%s\n", m->grid[i]);
}

static void	err(t_map *m)
{
	free_map(m);
	fprintf(stderr, "map error\n");
}
 
void	free_map(t_map *m)
{
	int	i;
 
	i = 0;
	while (m->grid && m->grid[i])
		free(m->grid[i++]);
	free(m->grid);
	m->grid = NULL;
	m->rows = 0;
}

/* ── parse helpers ───────────────────────────────────────────────────────── */

static int	parse_header(t_map *map, FILE *file)
{
	char	*line;
	size_t	buf_size;

	line = NULL;
	buf_size = 0;
	if (getline(&line, &buf_size, file) <= 0
		|| sscanf(line, "%d %c %c %c",
			&map->rows, &map->empty, &map->obstacle, &map->full) != 4
		|| map->rows <= 0
		|| map->empty == map->obstacle
		|| map->empty == map->full
		|| map->obstacle == map->full)
		return (free(line), 0);
	return (free(line), 1);
}

static int	valid_line_chars(t_map *map, char *line, int length)
{
	int	col;

	col = 0;
	while (col < length)
	{
		if (line[col] != map->empty && line[col] != map->obstacle)
			return (0);
		col++;
	}
	return (1);
}

static int	strip_newline_and_get_length(char *line)
{
	int	length;

	length = 0;
	while (line[length] && line[length] != '\n')
		length++;
	line[length] = '\0';
	return (length);
}

static char	*read_next_row(FILE *file)
{
	char	*line;
	size_t	buf_size;

	line = NULL;
	buf_size = 0;
	if (getline(&line, &buf_size, file) <= 0)
		return (free(line), NULL);
	return (line);
}

/* ── parse_map ───────────────────────────────────────────────────────────── */

int	parse_map(t_map *map, FILE *file)
{
	char	*line;
	int		row_index;
	int		line_length;

	map->grid = NULL;
	map->rows = 0;
	map->cols = -1;
	if (!parse_header(map, file))
		return (err(map), 0);
	if (!(map->grid = calloc(map->rows + 1, sizeof(char *))))
		return (err(map), 0);
	row_index = 0;
	while (row_index < map->rows)
	{
		line = read_next_row(file);
		if (!line)
			return (err(map), 0);
		line_length = strip_newline_and_get_length(line);
		if (map->cols == -1)
			map->cols = line_length;
		if (line_length == 0
			|| line_length != map->cols
			|| !valid_line_chars(map, line, line_length))
			return (free(line), err(map), 0);
		map->grid[row_index++] = line;
	}
	if (row_index != map->rows)
		return (err(map), 0);
	return (1);
}

/* ── solve helpers ───────────────────────────────────────────────────────── */

static int	min3(int a, int b, int c)
{
	return (a < b ? (a < c ? a : c) : (b < c ? b : c));
}

static int	**alloc_dp_table(int num_rows, int num_cols)
{
	int	**dp_table;
	int	row;

	if (!(dp_table = calloc(num_rows, sizeof(int *))))
		return (NULL);
	row = 0;
	while (row < num_rows)
	{
		if (!(dp_table[row] = calloc(num_cols, sizeof(int))))
			return (NULL);
		row++;
	}
	return (dp_table);
}

static void	free_dp_table(int **dp_table, int num_rows)
{
	int	row;

	row = 0;
	while (row < num_rows)
		free(dp_table[row++]);
	free(dp_table);
}

static int	square_size_at(int **dp_table, t_map *map, int row, int col)
{
	if (map->grid[row][col] == map->obstacle)
		return (0);
	if (row == 0 || col == 0)
		return (1);
	return (min3(dp_table[row - 1][col],
			dp_table[row][col - 1],
			dp_table[row - 1][col - 1]) + 1);
}

static void	fill_best_square(t_map *map, int bottom_row, int right_col, int size)
{
	int	row;
	int	col;

	row = bottom_row - size + 1;
	while (row <= bottom_row)
	{
		col = right_col - size + 1;
		while (col <= right_col)
			map->grid[row][col++] = map->full;
		row++;
	}
}

/* ── solve ───────────────────────────────────────────────────────────────── */

void	solve(t_map *map)
{
	int		**dp_table;
	int		best_size;
	int		best_bottom_row;
	int		best_right_col;
	int		row;
	int		col;

	if (!(dp_table = alloc_dp_table(map->rows, map->cols)))
		return ;
	best_size = 0;
	best_bottom_row = 0;
	best_right_col = 0;
	row = 0;
	while (row < map->rows)
	{
		col = 0;
		while (col < map->cols)
		{
			dp_table[row][col] = square_size_at(dp_table, map, row, col);
			if (dp_table[row][col] > best_size)
			{
				best_size = dp_table[row][col];
				best_bottom_row = row;
				best_right_col = col;
			}
			col++;
		}
		row++;
	}
	fill_best_square(map, best_bottom_row, best_right_col, best_size);
	free_dp_table(dp_table, map->rows);
}