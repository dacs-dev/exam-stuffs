/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 00:12:10 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/13 03:46:17 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "searchable_bag.hpp"

class set
{
	private:
		searchable_bag&	_bag;
		set();
		set(const set& other);
		set&	operator=(const set& other);
	public:
		set(searchable_bag& s_bag);
		~set();
		
		const searchable_bag&	get_bag();
		bool	has(int value) const;
		
		void insert (int);
		void insert (int *, int);
		void print() const;
		void clear();
};