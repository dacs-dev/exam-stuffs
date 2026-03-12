/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   searchable_tree_bag.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 00:29:00 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/12 03:34:43 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "searchable_tree_bag.hpp"


// bool	searchable_tree_bag::search(node* node, const int value) const
// {
// 	if (node == nullptr)
// 		return (false);
// 	if (node->value == value)
// 		return (true);
// 	else if (node->value > value)
// 		return (search(node->l, value));
// 	else
// 		return (search(node->r, value));
// }

// searchable_tree_bag::searchable_tree_bag()
// {
	
// }

// searchable_tree_bag::searchable_tree_bag(const searchable_tree_bag& other) : tree_bag(other)
// {
	
// }

// searchable_tree_bag&	searchable_tree_bag::operator=(const searchable_tree_bag& other)
// {
// 	if (this != &other)
// 		tree_bag::operator=(other);
// 	return (*this);
// }

// searchable_tree_bag::~searchable_tree_bag()
// {
	
// }


// bool	searchable_tree_bag::has(int value) const
// {
// 	return (search(this->tree, value));
// }


bool	searchable_tree_bag::search(node* node, int value) const
{
	if (node == nullptr)
		return (false);
	if (value == node->value)
		return (true);
	else if (value < node->value)
		return (search(node->l, value));
	else
		return (search(node->r, value));
}
searchable_tree_bag::searchable_tree_bag()
{
	
}
searchable_tree_bag::searchable_tree_bag(const searchable_tree_bag& other) : tree_bag(other)
{
	
}
searchable_tree_bag&	searchable_tree_bag::operator=(const searchable_tree_bag& other)
{
	if (this != &other)
		tree_bag::operator=(other);
	return (*this);
}
searchable_tree_bag::~searchable_tree_bag()
{
	
}

bool	searchable_tree_bag::has(int value) const
{
	return (search(this->tree, value));
}