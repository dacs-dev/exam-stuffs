/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   oldbigint.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 22:39:15 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/10 01:51:52 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include <string>
#include <sstream>
#include <iostream>

class bigint
{
	private:
		std::string	_str;

	public:
	
		bigint();
		bigint(unsigned int num);
		bigint(const bigint& other);
		bigint&	operator=(const bigint& other);
		~bigint();
		
		std::string	getStr() const;
		void	validate();

		bigint&	operator++();
		bigint		operator++(int);

		bigint&	operator+=(const bigint& toAdd);
		bigint	operator+(const bigint& toAdd) const;

		bigint&	operator>>=(const bigint& obj);
		bigint&	operator<<=(unsigned int shift);
		bigint	operator<<(unsigned int shift) const;

		bool operator<(const bigint& other) const;
		bool operator>(const bigint& other) const;
		bool operator==(const bigint& other) const;
		bool operator!=(const bigint& other) const;
		bool operator<=(const bigint& other) const;
		bool operator>=(const bigint& other) const;

};

std::ostream&	operator<<(std::ostream& os, const bigint& toPrint);