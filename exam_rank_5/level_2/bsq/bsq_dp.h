/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bsq_dp.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/16 03:35:46 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/16 03:36:47 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BSQ_DP_H
# define BSQ_DP_H
 
# include <stdlib.h>
# include <stdio.h>
 
typedef struct s_map
{
	char	**grid;
	int		rows;
	int		cols;
	char	empty;
	char	obstacle;
	char	full;
}	t_map;
 
int		parse_map(t_map *m, FILE *f);
void	solve(t_map *m);
void	print_map(t_map *m);
void	free_map(t_map *m);
 
#endif