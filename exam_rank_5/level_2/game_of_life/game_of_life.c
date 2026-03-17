/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_of_life.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 04:00:08 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/17 17:23:29 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// static void	printboard(char *board, int width, int height)
// {
// 	for (int y = 0; y < height; y++)
// 	{
// 		for (int x = 0; x < width; x++)
// 		{
// 			if (board[y * width + x])
// 				putchar('0');
// 			else
// 				putchar(' ');
// 		}
// 		putchar('\n');
// 	}
// }

// static int	countneighbours(char *board, int width, int height, int x, int y)
// {
// 	int	count = 0;
// 	for (int dy = -1; dy <= 1; dy++)
// 	{
// 		for (int dx = -1; dx <= 1; dx++)
// 		{
// 			if (dx == 0 && dy == 0)
// 				continue ;
// 			int neighbour_x = x + dx;
// 			int neighbour_y = y + dy;
// 			if ((neighbour_x >= 0 && neighbour_x < width) &&
// 				neighbour_y >= 0 && neighbour_y < height)
// 			{
// 				count += board[neighbour_y * width + neighbour_x];
// 			}
// 		}
// 	}
// 	return (count);
// }

// static void	step(char *board, char *nextboard, int width, int height)
// {
// 	for (int y = 0; y < height; y++)
// 	{
// 		for (int x = 0; x < width; x++)
// 		{
// 			int n = countneighbours(board, width, height, x, y);
// 			if (board[y * width + x])
// 			{
// 				if (n == 2 || n == 3)
// 					nextboard[y * width + x] = 1;
// 				else
// 					nextboard[y * width + x] = 0;
// 			}
// 			else
// 			{
// 				if (n == 3)
// 					nextboard[y * width + x] = 1;
// 				else
// 					nextboard[y * width + x] = 0;
// 			}
// 		}
// 	}
// }

// int main(int ac, char const *av[])
// {
// 	if (ac != 4)
// 		return (1);
	
// 	int width = atoi(av[1]);
// 	int height = atoi(av[2]);
// 	int iter = atoi(av[3]);

// 	char	*board = calloc(width * height, 1);
// 	char	*nextboard = calloc(width * height, 1);

// 	if (!board || !nextboard)
// 		return (1);
	
// 	int x = 0, y = 0, pencil = 0;
// 	char c;
	
// 	while (read(0, &c, 1) > 0)
// 	{
// 		if (c == 'x')
// 			pencil = !pencil;
// 		if (c == 'a' && x > 0)
// 			x--;
// 		else if (c == 'd' && x <= width)
// 			x++;
// 		else if (c == 's' && y > 0)
// 			y--;
// 		else if (c == 'w' && y <= height)
// 			y++;
// 		if (pencil)
// 			board[y * width + x] = 1;
// 	}

// 	for (int i = 0; i < iter; i++)
// 	{
// 		step(board, nextboard, width, height);
// 		char *tmp = board;
// 		board = nextboard;
// 		nextboard = tmp;
// 	}
// 	printboard(board, width, height);
// 	free(board);
// 	free(nextboard);
// 	return 0;
// }


static void	printboard(char *board, int width, int height)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (board[y * width + x])
				putchar('0');
			else
				putchar(' ');
		}
		putchar('\n');
	}
}

static int	countneighbours(char *board, int width, int height, int x, int y)
{
	int count = 0;

	for (int dy = -1; dy <= 1; dy++)
	{
		for (int dx = -1; dx <= 1; dx++)
		{
			if (dx == 0 && dy == 0)
				continue;
			int neighbour_x = x + dx;
			int neighbour_y = y + dy;
			if (neighbour_x >= 0 && neighbour_x < width &&
				neighbour_y >= 0 && neighbour_y < height)
			{
				count += board[neighbour_y * width + neighbour_x];
			}
		}
	}
	return (count);
}

static void	step(char *board, char *nextboard, int width, int height)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			int n = countneighbours(board, width, height, x, y);
			if (board[y * width + x])
			{
				if (n == 2 || n == 3)
					nextboard[y * width + x] = 1;
				else
					nextboard[y * width + x] = 0;
			}
			else
			{
				if (n == 3)
					nextboard[y * width + x] = 1;
				else
					nextboard[y * width + x] = 0;
			}
		}
	}
}

int main(int ac, char const *av[])
{
	if (ac != 4)
		return 1;
	
	int width = atoi(av[1]);
	int height = atoi(av[2]);
	int iter = atoi(av[3]);

	char *board = calloc(width * height, 1);
	char *nextboard = calloc(width * height, 1);
	
	if (!board || !nextboard)
		return 1;
	
	int x = 0, y = 0, pencil = 0;
	char c;
	
	while (read(0, &c, 1) > 0)
	{
		if (c == 'x')
			pencil = !pencil;
		if (c == 'a' && x > 0)
			x--;
		else if (c == 'd' && x <= width)
			x++;
		else if (c == 's' && y > 0)
			y--;
		else if (c == 'w' && y <= height)
			y++;
		if (pencil)
			board[y * width + x] = 1;
	}

	for (int i = 0; i < iter; i++)
	{
		step(board, nextboard, width, height);
		char *tmp = board;
		board = nextboard;
		nextboard = tmp;
	}

	printboard(board, width, height);
	free(board);
	free(nextboard);
	return 0;
}
