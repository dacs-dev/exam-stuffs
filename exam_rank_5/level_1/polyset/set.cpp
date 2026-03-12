/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 00:21:26 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/12 03:27:42 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "set.hpp"
#include "searchable_array_bag.hpp"

// set::set(searchable_bag& s_bag) : _bag(s_bag)
// {
	
// }
// set::~set()
// {
	
// }

// bool set::has(int value) const
// {
// 	return (_bag.has(value));
// }

// const searchable_bag&	set::get_bag()
// {
// 	return (_bag);
// }

// void set::insert (int value)
// {
// 	if (!(has(value)))
// 		return (_bag.insert(value));	
// }
// void set::insert (int * data, int size)
// {
// 	for (int i = 0; i < size; i++)
// 		insert(data[i]);
// }
// void set::print() const
// {
// 	_bag.print();
// }
// void set::clear()
// {
// 	_bag.clear();
// }


set::set(searchable_bag& s_bag) : _bag(s_bag)
{
	
}
set::~set()
{
	
}

const searchable_bag&	set::get_bag()
{
	return (_bag);
}
bool	set::has(int value) const
{
	return (_bag.has(value));
}

void set::insert (int value)
{
	if (!(_bag.has(value)))
		_bag.insert(value);
}
void set::insert (int *data, int size)
{
	for (int i = 0; i < size; i++)
		_bag.insert(data[i]);
}
void set::print() const
{
	_bag.print();
}
void set::clear()
{
	_bag.clear();
}