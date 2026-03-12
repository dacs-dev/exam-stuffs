/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 00:12:10 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/12 03:23:21 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "searchable_bag.hpp"

// class set
// {
// 	private:
// 		searchable_bag& _bag;
	
// 	public:
// 		set() = delete;
// 		set(const set& other) = delete;
// 		set&	operator=(const set& other) = delete;
// 		set(searchable_bag& s_bag);
// 		~set();

// 		bool	has(int) const;
// 		void	insert(int);
// 		void	insert(int *, int);
// 		void	print() const;
// 		void	clear();

// 		const searchable_bag&	get_bag();
		
// };

// class set
// {
// 	private:

	
// 	public:
// };


class set
{
	private:
		searchable_bag& _bag;
	
	public:
		set() = delete;
		set(const set& other) = delete;
		set&	operator=(const set& other) = delete;
		set(searchable_bag& s_bag);
		~set();
		
		const searchable_bag&	get_bag();
		bool	has(int value) const;

		void insert (int);
		void insert (int *, int);
		void print() const;
		void clear();
};