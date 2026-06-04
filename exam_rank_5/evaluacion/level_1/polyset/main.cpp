/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 00:33:40 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/11 00:33:41 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "searchable_bag.hpp"
#include "searchable_tree_bag.hpp"
#include "searchable_array_bag.hpp"
#include "set.hpp"


#include <iostream>

int main(int argc, char **argv)
{
	if (argc == 1)
		return 1;
	searchable_bag *t = new searchable_tree_bag;
	searchable_bag *a = new searchable_array_bag;

	for (int i = 1; i < argc; i++) {
		t->insert(atoi(argv[i]));
		a->insert(atoi(argv[i]));
	}
	t->print();
	a->print();

	for (int i = 1; i < argc; i++)
	{
		std::cout << t->has(atoi(argv[i])) << std::endl;
		std::cout << a->has(atoi(argv[i])) << std::endl;
		std::cout << t->has(atoi(argv[i]) - 1) << std::endl;
		std::cout << a->has(atoi(argv[i]) - 1) << std::endl;
	}

	t->clear();
	a->clear();

	const searchable_array_bag tmp(static_cast<searchable_array_bag &>(*a));
	tmp.print();
	tmp.has(1);

	set sa(*a);
	set st(*t);
	for (int i = 1; i < argc; i++)
	{
		st.insert(atoi(argv[i]));
		sa.insert(atoi(argv[i]));

		sa.has(atoi(argv[i]));
		sa.print();
		sa.get_bag().print();
		st.print();
		sa.clear();
		sa.insert((int[]){ 1, 2, 3, 4, }, 4);
		std::cout << std::endl;
	}

	return (0);
}