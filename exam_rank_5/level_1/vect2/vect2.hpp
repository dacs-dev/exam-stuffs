/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vect2.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 01:56:22 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/12 02:02:35 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>

// class vect2
// {
// 	private:
// 		int _x;
// 		int _y;
// 	public:
// 		vect2();
// 		vect2(int x, int y);
// 		vect2(const vect2& other);
// 		vect2&	operator=(const vect2& other);
// 		~vect2();

// 		int& operator[](int index);
// 		int operator[](int index) const;

// 		vect2& operator++();
// 		vect2 operator++(int);

// 		vect2& operator--();
// 		vect2 operator--(int);

// 		vect2& operator*=(int toMult);
// 		vect2 operator*(int toMult) const;

// 		vect2& operator+=(const vect2& toAdd);
// 		vect2 operator+(const vect2& toAdd) const;

// 		vect2& operator-=(const vect2& toSub);
// 		vect2 operator-(const vect2& toSub) const;
		
// 		bool	operator==(const vect2& other) const;
// 		bool	operator!=(const vect2& other) const;

// 		vect2	operator-() const;
// };

// vect2	operator*(int num, const vect2& toMult);
// std::ostream& operator<<(std::ostream& os, const vect2& toPrint);

//22

class vect2
{
	private:
		int _x;
		int _y;
	public:
		vect2();
		vect2(int x, int y);
		vect2(const vect2& other);
		vect2&	operator=(const vect2& other);
		~vect2();
		
		int&	operator[](int index);
		int		operator[](int index) const;

		vect2&	operator++();
		vect2	operator++(int);

		vect2&	operator--();
		vect2	operator--(int);

		vect2&	operator*=(int toMult);
		vect2	operator*(int toMult) const;

		vect2&	operator+=(const vect2& toAdd);
		vect2	operator+(const vect2& toAdd) const;

		vect2&	operator-=(const vect2& toSub);
		vect2	operator-(const vect2& toSub) const;

		bool	operator==(const vect2& other) const;
		bool	operator!=(const vect2& other) const;

		vect2	operator-() const;
};

vect2	operator*(int num, const vect2& toMult);
std::ostream&	operator<<(std::ostream& os, const vect2& toPrint);