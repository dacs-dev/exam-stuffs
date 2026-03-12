/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   searchable_array_bag.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 00:18:31 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/12 03:14:13 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "array_bag.hpp"
#include "searchable_bag.hpp"



// class searchable_array_bag : public searchable_bag, public array_bag
// {
// 	public:
// 		searchable_array_bag();
// 		searchable_array_bag(const searchable_array_bag& other);
// 		searchable_array_bag& operator=(const searchable_array_bag& other);
// 		~searchable_array_bag();
		
// 		bool	has(int value) const;
// };

class searchable_array_bag : public searchable_bag, public array_bag
{
	public:
		searchable_array_bag();
		searchable_array_bag(const searchable_array_bag& other);
		searchable_array_bag&	operator=(const searchable_array_bag& other);
		~searchable_array_bag();
		
		bool has(int value) const;
};