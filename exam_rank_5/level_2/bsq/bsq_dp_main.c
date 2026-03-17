/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsq_dp_main.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 03:36:27 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/16 03:36:35 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bsq_dp.h"

static void	process(FILE *f, int last)
{
	t_map	m;

	if (parse_map(&m, f))
	{
		solve(&m);
		print_map(&m);
		free_map(&m);
	}
	if (!last)
		fprintf(stdout, "\n");
}

int	main(int argc, char **argv)
{
	FILE	*f;

	if (argc == 1)
	{
		process(stdin, 1);
		return (0);
	}
	for (int i = 1; i < argc; i++)
	{
		if (!(f = fopen(argv[i], "r")))
		{
			fprintf(stderr, "map error\n");
			if (i < argc - 1)
				fprintf(stdout, "\n");
			continue ;
		}
		process(f, i == argc - 1);
		fclose(f);
	}
	return (0);
}