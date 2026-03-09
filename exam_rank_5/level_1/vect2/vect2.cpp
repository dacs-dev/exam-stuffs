/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vect2.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 01:56:25 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/09 07:27:58 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vect2.hpp"

// vect2::vect2() : _x(0), _y(0)
// {
	
// }

// vect2::vect2(int x, int y) : _x(x), _y(y)
// {
	
// }

// vect2::vect2(const vect2& other) : _x(other._x), _y(other._y)
// {
	
// }

// vect2&	vect2::operator=(const vect2& other)
// {
	
// }
// vect2::~vect2()
// {
	
// }

// int& vect2::operator[](int index)
// {
	
// }

// int vect2::operator[](int index) const
// {
	
// }

// vect2& vect2::operator++()
// {
	
// }

// vect2 vect2::operator++(int)
// {
	
// }

// vect2& vect2::operator--()
// {
	
// }
// vect2 vect2::operator--(int)
// {
	
// }

// vect2& vect2::operator*=(int toMult)
// {
	
// }
// vect2 vect2::operator*(int toMult) const
// {
	
// }

// vect2& vect2::operator+=(const vect2& toAdd)
// {
	
// }
// vect2 vect2::operator+(const vect2& toAdd) const
// {
	
// }

// vect2& vect2::operator-=(const vect2& toSub)
// {
	
// }
// vect2 vect2::operator-(const vect2& toSub) const
// {
	
// }

// bool	vect2::operator==(const vect2& other) const
// {
	
// }

// bool	vect2::operator!=(const vect2& other) const
// {
	
// }

// vect2	vect2::operator-() const
// {
	
// }


// vect2	operator*(int num, const vect2& toMult)
// {
	
// }
// std::ostream& operator<<(std::ostream& os, const vect2& toPrint)
// {
	
// }

vect2::vect2() : _x(0), _y(0)
{
	
}

vect2::vect2(int x, int y) : _x(x), _y(y)
{
	
}

vect2::vect2(const vect2& other) : _x(other._x), _y(other._y)
{
	
}

vect2&	vect2::operator=(const vect2& other)
{
	if (this != &other)
	{
		_x = other._x;
		_y = other._y;
	}
	return (*this);
}
vect2::~vect2()
{
	
}

int& vect2::operator[](int index)
{
	if (index == 0)
		return (_x);
	else
		return (_y);
}

int vect2::operator[](int index) const
{
	if (index == 0)
		return (_x);
	else
		return (_y);
}

vect2& vect2::operator++()
{
	_x++;
	_y++;
	return (*this);
}

vect2 vect2::operator++(int)
{
	vect2	tmp(*this);
	_x++;
	_y++;
	return (tmp);
}

vect2& vect2::operator--()
{
	_x--;
	_y--;
	return (*this);
}
vect2 vect2::operator--(int)
{
	vect2	tmp(*this);
	_x--;
	_y--;
	return (tmp);
}

vect2& vect2::operator*=(int toMult)
{
	_x *= toMult;
	_y *= toMult;
	return (*this);
}

vect2 vect2::operator*(int toMult) const
{
	vect2	tmp(*this);
	tmp._x = _x * toMult;
	tmp._y = _y * toMult;
	return (tmp);
}

vect2& vect2::operator+=(const vect2& toAdd)
{
	_x += toAdd._x;
	_y += toAdd._y;
	return (*this);
}

vect2 vect2::operator+(const vect2& toAdd) const
{
	vect2	tmp(*this);
	tmp._x = _x + toAdd._x;
	tmp._y = _y + toAdd._y;
	return (tmp);
}

vect2& vect2::operator-=(const vect2& toSub)
{
	_x -= toSub._x;
	_y -= toSub._y;
	return (*this);
}
vect2 vect2::operator-(const vect2& toSub) const
{
	vect2	tmp(*this);
	tmp._x = _x - toSub._x;
	tmp._y = _y - toSub._y;
	return (tmp);
}

bool	vect2::operator==(const vect2& other) const
{
	if (_x == other._x && _y == other._y)
		return (true);
	else
		return (false);
}

bool	vect2::operator!=(const vect2& other) const
{
	if (_x == other._x && _y == other._y)
		return (false);
	else
		return (true);
}

vect2	vect2::operator-() const
{
	vect2	tmp(*this);
	tmp._x = -_x;
	tmp._y = -_y;
	return (tmp);
}


vect2	operator*(int num, const vect2& toMult)
{
	vect2	tmp(toMult);
	tmp *= num;
	return (tmp);
}

std::ostream& operator<<(std::ostream& os, const vect2& toPrint)
{
	os << "{" << toPrint[0] << ", " << toPrint[1] << "}";
	return (os);
}