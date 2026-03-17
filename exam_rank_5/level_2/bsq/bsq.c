/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsq.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 03:56:51 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/17 17:25:04 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bsq.h"

//UTILS
//void print_map(t_map * map);
//static void err(t_map *m)
//void free_map(t_map *m)

//PARSE
//static int parse_header(t_map *map, FILE *file)
//static int valid_line_chars(t_map *map, char *line, int length)
//static int strip_nl_get_len(char *line)
//static char *read_next_row(FILE *file)
//parse_map(t_map *map, FILE *file)

//SOLVE
//static int min3(int a, int b, int c)
//static int **alloc_dp_table(int num_rows, int num_cols)
//static void free_dp_table(int **dp_table, int num_rows)
//static int square_size_at(int **dp_table, t_map *map, int row, int col)
//static void fill_best_square(t_map *map, int bottom_row, int right_col, int size)
//solve(t_map *map)

void	print_map(t_map *map)
{
	for (int i = 0; i < map->rows; i++)
		fprintf(stdout, "%s\n", map->grid[i]);
}

static void	err(t_map *map)
{
	free_map(map);
	fprintf(stderr, "map error\n");
}

void	free_map(t_map *map)
{
	int i = 0;
	
	while (map->grid && map->grid[i])
		free(map->grid[i++]);
	free(map->grid);
	map->grid = NULL;
	map->rows = 0;
}

//PARSE
//static int parse_header
//static int valid_line_chars
//static int strip_nl_get_len
//static char *read_next_row
//parse_map

static int parse_header(t_map *map, FILE *file)
{
	char *line = NULL;
	size_t buf_size = 0;
	if (getline(&line, &buf_size, file) <= 0
		||sscanf(line, "%d %c %c %c", &map->rows, &map->empty, &map->obstacle, &map->full) != 4
		|| map->rows <= 0
		|| map->empty == map->obstacle
		|| map->full == map->empty
		|| map->obstacle == map->full)
		return (free(line), 0);
	return (free(line), 1);
}

static int valid_line_chars(t_map *map, char *line, int length)
{
	int col = 0;
	while (col < length)
	{
		if (line[col] != map->empty && line[col] != map->obstacle)
			return (0);
		col++;
	}
	return (1);
}

static int	strip_nl_get_len(char *line)
{
	int len = 0;
	
	while (line[len] && line[len] != '\n')
		len++;
	line[len] = '\0';
	return (len);
}

static char	*read_next_row(FILE *file)
{
	char	*line = NULL;
	size_t	buf_size = 0;
	
	if (getline(&line, &buf_size, file) <= 0)
		return (free(line), NULL);
	return (line);
}

int	parse_map(t_map *map, FILE *file)
{
	char	*line;
	int row_index = 0;
	int line_len;

	map->grid = NULL;
	map->rows = 0;
	map->cols = -1;
	if (!parse_header(map, file))
		return (err(map), 0);
	if (!(map->grid = calloc(map->rows + 1, sizeof(char *))))
		return (err(map), 0);
	while (row_index < map->rows)
	{
		line = read_next_row(file);
		if (!line)
			return (err(map), 0);
		line_len = strip_nl_get_len(line);
		if (map->cols == -1)
			map->cols = line_len;
		if (line_len == 0 || line_len != map->cols
			|| !valid_line_chars(map, line, line_len))
			return (err(map), 0);
		map->grid[row_index++] = line;
	}
	if (row_index != map->rows)
		return (err(map), 0);
	return (1);
}
//SOLVE
//static int min3
//static int **alloc_dp_table
//static void free_dp_table
//static int square_size_at
//static void fill_best_square
//solve

static int	min3(int a, int b, int c)
{
	return (a < b ? (a < c ? a : c) : (b < c ?  : c));
}

static int	**alloc_dp_table(int num_rows, int num_cols)
{
	int	**dp_table;
	int row = 0;
	
	if (!(dp_table = calloc(num_rows, sizeof(int *))))
		return (NULL);
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
	int row = 0;

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
	return (min3(dp_table[row -1][col],
			dp_table[row][col - 1],
			dp_table[row - 1][ col - 1]) + 1);
}

static void	fill_best_square(t_map *map, int bottom_row, int right_col, int size)
{
	int row = bottom_row - size + 1;
	int col;

	while (row <= bottom_row)
	{
		col = right_col - size + 1;
		while (col <= right_col)
			map->grid[row][col++] = map->full;
		row++;
	}
}

void	solve(t_map *map)
{
	int	**dp_table;
	int best_size = 0;
	int best_bottom_row = 0;
	int best_right_col = 0;
	int row = 0;
	int col;

	if (!(dp_table = alloc_dp_table(map->rows, map->cols)))
		return ;
	
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

//UTILS
//void print_map(t_map *map);
//static void err(t_map *map)
//void free_map(t_map *map)

void	print_map(t_map *map)
//PARSE
//static int parse_header(t_map *map, FILE *file)
//static int valid_line_chars(t_map *map, char *line, int length)
//static int strip_nl_get_len(char *line)
//static char *read_next_row(FILE *file)
//parse_map(t_map *map, FILE *file)

//SOLVE
//static int min3(int a, int b, int c)
//static int **alloc_dp_table(int num_rows, int num_cols)
//static void free_dp_table(int **dp_table, int num_rows)
//static int square_size_at(int **dp_table, t_map *map, int row, int col)
//static void fill_best_square(t_map *map, int bottom_row, int right_col, int size)
//solve(t_map *map)



void	printmap(t_map *map)
{
	for (int i = 0; i < map->rows; i++)
		fprintf(stdout, "%s\n", map->grid[i]);
}

static void	err(t_map *map)
{
	free_map(map);
	fprintf(stderr, "map error\n");
}

void	free_map(t_map *map)
{
	for (int i = 0; i < map->rows; i++)
		free(map->grid[i]);
	free(map->grid);
	map->grid = NULL;
	map->rows = 0;
}



// static int parse_header(t_map *map, FILE *file)
// {
// 	char *line = NULL;
// 	size_t buf_size = 0;
// 	if (getline(&line, &buf_size, file) <= 0
// 		|| sscanf(line, "%d %c %c %c", &map->rows, &map->empty, &map->obstacle, &map->full) != 4
// 		|| map->rows <= 0
// 		|| map->empty == map->full
// 		|| map->full == map->obstacle
// 		|| map->obstacle == map->empty)
// 		return (free(line), 0);
// 	return (free(line), 1);
// }


static int	parse_header(t_map *map, FILE *file)
{
	char	*line = NULL;
	size_t	buf_len = 0;
	
	if (getline(&line, &buf_len, file) <= 0
		|| sscanf(line, "%d %c %c %c", &map->rows, &map->empty, &map->obstacle, &map->full) != 4
		|| map->rows <= 0
		|| map->empty == map->full
		|| map->full == map->obstacle
		|| map->obstacle == map->empty)
		return (free(line), 0);
	return (free(line), 1);
}
// static int valid_line_chars(t_map *map, FILE *file, char * line, int len)
// {
// 	int col = 0;

// 	while (col < len)
// 	{
// 		if (line[col] != map->empty && line[col] != map->obstacle)
// 			return (0);
// 		col++;
// 	}
// 	return (1);
// }

static int	valid_line_chars(t_map *map, char *line, int len)
{
	int col = 0;

	while (col < len)
	{
		if (line[col] != map->empty && line[col] != map->obstacle)
			return (0);
		col++;
	}
	return (1);
}

// static int strip_nl_get_len(char *line)
// {
// 	int len = 0;
	
// 	while (line[len] && line[len != '\n'])
// 		len++;
// 	line[len] = '\0';
// 	return (len);
// }


int static strip_nl_get_len(char *line)
{
	int len = 0;
	
	while (line[len] && line[len] != '\n')
		len++;
	line[len] = '\0';
	return (len);
}
static char *read_next_row(FILE *file)
{
	char *line = NULL;
	size_t buf_size = 0;

	if (getline(&line, &buf_size, file) <= 0)
		return (free(line), NULL);
	return (line);
}

int parse_map(t_map *map, FILE *file)
{
	char	*line;
	int		row_index = 0;
	int		line_len;

	map->grid = NULL;
	map->rows = 0;
	map->cols = -1;
	
	if (!(parse_header(map, file)))
		return (err(map), 0);
	if (!(map->grid = calloc(map->rows + 1, sizeof(char *))))
		return (err(map), 0);
	while (row_index < map->rows)
	{
		line = read_next_row(file);
		if (!line)
			return (err(map), 0);
		line_len = strip_nl_get_len(line);
		if (map->cols == -1)
			map->cols = line_len;
		if (line_len == 0 || line_len != map->cols
			|| !valid_line_chars(map, line, line_len))
			return (err(map), 0);
		map->grid[row_index++] = line;
	}
	if (row_index != map->rows)
		return (err(map), 0);
	return (1);
}