/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsq.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 03:40:14 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/19 05:38:09 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef struct s_map
{
	int	rows;
	int	cols;
	char	empty;
	char	obstacle;
	char	full;
	char	**grid;
} t_map;

void	print_map(t_map *map);
void	free_map(t_map *map);
int		parse_map(t_map *map, FILE *file);
void	solve(t_map *map);