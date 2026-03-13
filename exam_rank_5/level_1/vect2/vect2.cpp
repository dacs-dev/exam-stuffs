/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vect2.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 01:56:25 by luferna3          #+#    #+#             */
/*   Updated: 2026/03/13 03:48:35 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vect2.hpp"

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

int		vect2::operator[](int index) const
{
	if (index == 0)
	{
		return (_x);
	}
	else
		return (_y);
}

int&	vect2::operator[](int index)
{
	if (index == 0)
	{
		return (_x);
	}
	else
		return (_y);
}

vect2&	vect2::operator++()
{
	_x++;
	_y++;
	return (*this);
}

vect2	vect2::operator++(int)
{
	vect2 tmp(*this);
	_x++;
	_y++;
	return (tmp);
}

vect2&	vect2::operator--()
{
	_x--;
	_y--;
	return (*this);
}

vect2	vect2::operator--(int)
{
	vect2 tmp(*this);
	_x--;
	_y--;
	return (tmp);
}

vect2& vect2::operator*=(int to_mult)
{
	_x *= to_mult;
	_y *= to_mult;
	return (*this);
}

vect2 vect2::operator*(int to_mult) const
{
	vect2 tmp(*this);
	tmp._x = _x * to_mult;
	tmp._y = _y * to_mult;
	return (tmp);
}
		
vect2& vect2::operator+=(const vect2& to_add)
{
	_x += to_add._x;
	_y += to_add._y;
	return (*this);
}

vect2 vect2::operator+(const vect2& to_add) const
{
	vect2 tmp(*this);
	tmp._x = _x + to_add._x;
	tmp._y = _y + to_add._y;
	return (tmp);
}
		
vect2& vect2::operator-=(const vect2& to_sub)
{
	_x -= to_sub._x;
	_y -= to_sub._y;
	return (*this);
}

vect2 vect2::operator-(const vect2& to_sub) const
{
	vect2 tmp(*this);
	tmp._x = _x - to_sub._x;
	tmp._y = _y - to_sub._y;
	return (tmp);
}

vect2 vect2::operator-() const
{
	vect2 tmp(*this);
	tmp._x = -_x;
	tmp._y = -_y;
	return (tmp);
}

bool vect2::operator!=(const vect2& other) const
{
	if (_x == other._x && _y == other._y)
		return (false);
	else
		return (true);
}

bool vect2::operator==(const vect2& other) const
{
	if (_x == other._x && _y == other._y)
		return (true);
	else
		return (false);
}

vect2 operator*(int num, const vect2& to_mult)
{
	vect2 temp(to_mult);
	temp *= num;
	return temp;
}

std::ostream& operator<<(std::ostream& os, const vect2& toPrint)
{
	os << "{" << toPrint[0] << ", " << toPrint[1] << "}";
	return os;
}