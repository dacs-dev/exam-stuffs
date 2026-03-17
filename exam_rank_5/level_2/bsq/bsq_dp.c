/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsq_dp.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 03:36:02 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/16 03:36:16 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bsq_dp.h"
 
static void	err(t_map *m)
{
	free_map(m);
	fprintf(stderr, "map error\n");
}
 
static int	min3(int a, int b, int c)
{
	return (a < b ? (a < c ? a : c) : (b < c ? b : c));
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
 
int	parse_map(t_map *m, FILE *f)
{
	char	*line;
	size_t	n;
	int		i;
	int		len;
 
	line = NULL;
	n = 0;
	m->grid = NULL;
	m->rows = 0;
	m->cols = -1;
	if (getline(&line, &n, f) <= 0
		|| sscanf(line, "%d %c %c %c",
			&m->rows, &m->empty, &m->obstacle, &m->full) != 4
		|| m->rows <= 0
		|| m->empty == m->obstacle
		|| m->empty == m->full
		|| m->obstacle == m->full)
		return (free(line), err(m), 0);
	free(line);
	if (!(m->grid = calloc(m->rows + 1, sizeof(char *))))
		return (err(m), 0);
	i = 0;
	line = NULL;
	n = 0;
	while (i < m->rows && getline(&line, &n, f) > 0)
	{
		len = 0;
		while (line[len] && line[len] != '\n')
		{
			if (line[len] != m->empty && line[len] != m->obstacle)
				return (free(line), err(m), 0);
			len++;
		}
		line[len] = '\0';
		if (m->cols == -1)
			m->cols = len;
		if (len != m->cols || len == 0)
			return (free(line), err(m), 0);
		m->grid[i++] = line;
		line = NULL;
		n = 0;
	}
	free(line);
	if (i != m->rows)
		return (err(m), 0);
	return (1);
}
 
void	solve(t_map *m)
{
	int		**dp;
	int		best;
	int		br;
	int		bc;
	int		v;
 
	if (!(dp = calloc(m->rows, sizeof(int *))))
		return ;
	for (int r = 0; r < m->rows; r++)
		if (!(dp[r] = calloc(m->cols, sizeof(int))))
			return ;
	best = 0;
	br = 0;
	bc = 0;
	for (int r = 0; r < m->rows; r++)
	{
		for (int c = 0; c < m->cols; c++)
		{
			if (m->grid[r][c] == m->obstacle)
				dp[r][c] = 0;
			else if (r == 0 || c == 0)
				dp[r][c] = 1;
			else
				dp[r][c] = min3(dp[r-1][c], dp[r][c-1], dp[r-1][c-1]) + 1;
			if (dp[r][c] > best)
			{
				best = dp[r][c];
				br = r;
				bc = c;
			}
		}
	}
	for (int r = br - best + 1; r <= br; r++)
		for (int c = bc - best + 1; c <= bc; c++)
			m->grid[r][c] = m->full;
	for (int r = 0; r < m->rows; r++)
		free(dp[r]);
	free(dp);
}
 
void	print_map(t_map *m)
{
	for (int i = 0; i < m->rows; i++)
		fprintf(stdout, "%s\n", m->grid[i]);
}