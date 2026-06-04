/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree_bag.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/11 00:13:42 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/11 00:14:08 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "bag.hpp"

class tree_bag : virtual public bag {
protected:
	struct node {
	  node *l;
	  node *r;
	  int value;
	};

	node *tree;

public:
	tree_bag();
	tree_bag(const tree_bag &);
	tree_bag  &operator=(const tree_bag &);
	~tree_bag();

	node *extract_tree();
	void set_tree(node *);

	virtual void insert(int);
	virtual void insert(int *array, int size);
	virtual void print() const;
	virtual void clear();

private:
	static void destroy_tree(node *);
	static void print_node(node *);
	static node *copy_node(node *);
};