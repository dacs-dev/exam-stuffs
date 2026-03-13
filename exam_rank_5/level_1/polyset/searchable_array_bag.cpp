/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   searchable_array_bag.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 00:18:24 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/13 03:49:21 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "searchable_array_bag.hpp"

searchable_array_bag::searchable_array_bag()
{
	
}

searchable_array_bag::searchable_array_bag(const searchable_array_bag& other) : array_bag(other)
{
	
}

searchable_array_bag&	searchable_array_bag::operator=(const searchable_array_bag& other)
{
	if (this != &other)
		array_bag::operator=(other);
	return (*this);
}

searchable_array_bag::~searchable_array_bag()
{
	
}

bool	searchable_array_bag::has(int value) const
{
	for (int i = 0; i < size; i++)
	{
		if (data[i] == value)
			return (true);
	}
	return (false);
}
