/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 03:39:28 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/19 07:57:23 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bsq.h"

// static void	process(FILE *file, int is_last)
// {
// 	t_map map;
	
// 	if (!parse_map(&map, file))
// 	{
// 		if (!is_last)
// 			fprintf(stdout, "\n");
// 		return ;
// 	}
// 	solve(&map);
// 	print_map(&map);
// 	free_map(&map);
// 	if (!is_last)
// 		fprintf(stdout, "\n");
// }

// int main(int ac, char const *av[])
// {
// 	FILE *file;

// 	if (ac == 1)
// 		return (process(stdin, 1), 0);
	
// 	int i = 1;
// 	while (i < ac)
// 	{
// 		file = fopen(av[i], "r");
// 		if (!file)
// 			fprintf(stderr, "map error");
// 		else
// 		{
// 			process(file, i == ac - 1);
// 			fclose(file);
// 		}
// 		if (i < ac - 1 && !file)
// 			fprintf(stdout, "\n");
// 		i++;
// 	}
// 	return 0;
// }

static void	process(FILE *file, int is_last)
{
	t_map map;

	if (!parse_map(&map, file))
	{
		if (!is_last)
			fprintf(stdout, "\n");
	}
	solve(&map);
	print_map(&map);
	free_map(&map);
	if (!is_last)
			fprintf(stdout, "\n");
}

int main(int ac, char const *av[])
{
	FILE *file;
	
	if (ac == 1)
		return (process(stdin, 1), 0);
	for (int i = 1; i < ac; i++)
	{
		file = fopen(av[i], "r");
		if (!file)
			fprintf(stderr, "map error\n");
		else
		{
			process(file, i == ac - 1);
			fclose(file);
		}
		if (i < ac - 1 && !file)
			fprintf(stdout, "\n");
		i++;
	}
	return 0;
}
