/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   searchable_tree_bag.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 00:19:58 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/12 03:20:31 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "tree_bag.hpp"
#include "searchable_bag.hpp"


// class searchable_tree_bag : public tree_bag, public searchable_bag
// {
// 	private:
// 		bool search(node* node, const int value) const;
// 	public:
// 		searchable_tree_bag();
// 		searchable_tree_bag(const searchable_tree_bag& source);
// 		searchable_tree_bag& operator=(const searchable_tree_bag& source);
// 		bool has(int) const;
// 		~searchable_tree_bag();
// };


class searchable_tree_bag : public searchable_bag, public tree_bag
{
	private:
		bool	search(node* node, int value) const;
	public:
		searchable_tree_bag();
		searchable_tree_bag(const searchable_tree_bag& other);
		searchable_tree_bag&	operator=(const searchable_tree_bag& other);
		~searchable_tree_bag();
		
		bool	has(int value) const;
};