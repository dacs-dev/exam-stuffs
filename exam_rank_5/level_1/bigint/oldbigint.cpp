
#include "oldbigint.hpp"

bigint::bigint() : _str("0")
{
	
}

bigint::bigint(unsigned int n)
{
	if (n == 0)
	{
		_str = "0";
		return ;
	}
	std::ostringstream	ss;
	ss << n;
	_str = ss.str();
	validate();
}

bigint::bigint(const bigint& other) :_str(other.getStr())
{
	
}

bigint& bigint::operator=(const bigint& other)
{
	if (this != &other)
		_str = other.getStr();
	return (*this);
}

bigint::~bigint()
{
	
}


std::string bigint::getStr() const
{
	return (_str);
}

void bigint::validate()
{
	if (_str.empty())
		return ;
	int i = 0;
	while (_str[i] && _str[i] == '0')
		i++;
	if (i > 0)
		_str.erase(0, i);
}

bigint& bigint::operator++()
{
	*this += bigint(1);
	return (*this);
}

bigint bigint::operator++(int)
{
	bigint	tmp(*this);
	*this += bigint(1);
	return (tmp);
}

bigint& bigint::operator<<=(unsigned int shift)
{
	if (_str == "0")
		return (*this);
	_str.append(shift, '0');
	return (*this);
}

bigint bigint::operator<<(unsigned int shift) const
{
	bigint	tmp(*this);
	tmp._str.append(shift, '0');
	return (tmp);
}


static unsigned long long str_to_number(std::string str)
{
	unsigned long long result = 0;
	if (str == "0")
		return (0);
	for (size_t i = 0; i < str.size(); i++)
		result = result * 10 + (str[i] - '0');
	return (result);
}

bigint& bigint::operator>>=(const bigint& obj)
{
	unsigned long long	shift = str_to_number(obj._str);
	if (shift >= _str.size())
		_str = "0";
	else
		_str.erase(_str.size() - shift, shift);
	return (*this);
	
}

bigint bigint::operator+(const bigint& other) const
{
	bigint tmp(*this);
	tmp += other;
	return tmp;
}

bigint& bigint::operator+=(const bigint& other)
{
    int i = _str.size() - 1;
    int j = other._str.size() - 1;
    int carry = 0;

    std::string result;

    while (i >= 0 || j >= 0 || carry)
    {
        int d1 = (i >= 0) ? _str[i] - '0' : 0;
        int d2 = (j >= 0) ? other._str[j] - '0' : 0;
        int sum = d1 + d2 + carry;
        result += (sum % 10) + '0';
        carry = sum / 10;
        i--;
        j--;
    }

    std::reverse(result.begin(), result.end());
    _str = result;
    return *this;
}

bool bigint::operator<(const bigint& obj) const
{
    if (_str.size() != obj._str.size())
        return _str.size() < obj._str.size();

    return _str < obj._str;
}

bool bigint::operator>(const bigint& obj) const
{
    return obj < *this;
}

bool bigint::operator==(const bigint& obj) const
{
    return _str == obj._str;
}

bool bigint::operator!=(const bigint& obj) const
{
    return !(*this == obj);
}

bool bigint::operator<=(const bigint& obj) const
{
    return !(*this > obj);
}

bool bigint::operator>=(const bigint& obj) const
{
    return !(*this < obj);
}


std::ostream& operator<<(std::ostream& os, const bigint& toPrint)
{
	os << toPrint.getStr();
	return (os);
}
