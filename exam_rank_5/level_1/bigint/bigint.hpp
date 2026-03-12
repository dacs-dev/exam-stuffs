/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bigint.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 03:07:04 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/12 02:19:38 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <sstream>
#include <iostream>

// class bigint
// {
// 	private:
// 		std::string	_str;

// 	public:
	
// 		bigint();
// 		bigint(unsigned int num);
// 		bigint(const bigint& other);
// 		bigint&	operator=(const bigint& other);
// 		~bigint();
		
// 		std::string	get_str() const;
// 		void	validate();

// 		bigint&	operator++();
// 		bigint		operator++(int);

// 		bigint&	operator+=(const bigint& toAdd);
// 		bigint	operator+(const bigint& toAdd) const;

// 		bigint&	operator>>=(const bigint& obj);
// 		bigint&	operator<<=(unsigned int shift);
// 		bigint	operator<<(unsigned int shift) const;

// 		bool operator<(const bigint& other) const;
// 		bool operator>(const bigint& other) const;
// 		bool operator==(const bigint& other) const;
// 		bool operator!=(const bigint& other) const;
// 		bool operator<=(const bigint& other) const;
// 		bool operator>=(const bigint& other) const;

// };

// std::ostream&	operator<<(std::ostream& os, const bigint& toPrint);

//21

class bigint
{
	private:
		std::string	_str;
		
	public:
		bigint();
		bigint(unsigned int n);
		bigint(const bigint& other);
		bigint&	operator=(const bigint& other);
		~bigint();
		
		std::string	getStr() const;
		void	validate();

		bigint&	operator++();
		bigint	operator++(int);

		bigint&	operator+=(const bigint& other);
		bigint	operator+(const bigint& other) const;

		bigint&	operator>>=(const bigint& obj);
		bigint&	operator<<=(unsigned int shift);
		bigint	operator<<(unsigned int shift) const;

		bool	operator==(const bigint& other) const;
		bool	operator!=(const bigint& other) const;
		bool	operator<=(const bigint& other) const;
		bool	operator>=(const bigint& other) const;
		bool	operator<(const bigint& other) const;
		bool	operator>(const bigint& other) const;
};

std::ostream&	operator<<(std::ostream& os, const bigint& toPrint);