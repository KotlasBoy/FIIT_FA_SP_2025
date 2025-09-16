#include "../include/fraction.h"

static const fraction PI(245850922_bi, 78256779_bi);
static const fraction HALF_PI = PI / fraction(2_bi, 1_bi);

void fraction::optimise()
{
    if (_denominator == big_int(0))
        throw std::invalid_argument("Denominator is zero");

    if (!_denominator.operator bool())
        throw std::invalid_argument("Zero denominator");

    if (_denominator < big_int(0))                      //TODO:FIXME:
    {
        _numerator = big_int(0) - _numerator;
        _denominator = big_int(0) - _denominator;
    }

    big_int up = _numerator;
    big_int down = _denominator;

    big_int gg = gcd(up, down);

    if (gg != big_int(1) && (gg != big_int(0)))
    {
        _numerator /= gg;
        _denominator /= gg;
    }
}

big_int gcd(big_int a, big_int b)
{
    if (a < 0)
        a = big_int(0) - a;
    if (b < 0)
        b = big_int(0) - b;
    while (b != big_int(0))
    {
        big_int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

fraction fraction::abs() const
{
    return fraction(_numerator, _denominator < 0_bi ? -_denominator : _denominator);
}

template<std::convertible_to<big_int> f, std::convertible_to<big_int> s>
fraction::fraction(f &&numerator, s &&denominator) : _numerator(std::forward<f>(numerator)), _denominator(std::forward<s>(denominator))
{
    if (_denominator == big_int(0))
        throw std::invalid_argument("Denominator cannot be zero");
    optimise();
}

fraction::fraction(pp_allocator<big_int::value_type>) : _numerator(0_bi), _denominator(0_bi)
{}

fraction &fraction::operator+=(fraction const &other) &
{
    big_int new_numerator = _numerator * other._denominator + other._numerator * _denominator;
    big_int new_denominator = _denominator * other._denominator;
    _numerator = new_numerator;
    _denominator = new_denominator;
    optimize();                                         //TODO:FIXME:
    return *this;
}

fraction fraction::operator+(fraction const &other) const
{
    fraction result = *this;
    result += other;
    return result;
}

fraction &fraction::operator-=(fraction const &other) &
{
    big_int new_numerator = _numerator * other._denominator - other._numerator * _denominator;
    big_int new_denominator = _denominator * other._denominator;
    _numerator = new_numerator;
    _denominator = new_denominator;
    optimise();                                                         //TODO:FIXME:
    return *this;
}

fraction fraction::operator-(fraction const &other) const
{
    fraction result = *this;
    result -= other;
    return result;
}

fraction &fraction::operator*=(fraction const &other) &
{
    _numerator = _numerator * other._numerator;
    _denominator = _denominator * other._denominator;
    optimise();
    return *this;
}

fraction fraction::operator*(fraction const &other) const
{
    fraction result = *this;
    result *= other;
    return result;
}

fraction &fraction::operator/=(fraction const &other) &
{
    if(!other._numerator)
        throw std::invalid_argument("Division by zero");
    _numerator *= other._denominator;
    _denominator *= other._numerator;

    if (_denominator < 0_bi)
    {
        _numerator = -_numerator;
        _denominator = -_denominator;
    }

    optimise();
    return *this;
}

fraction fraction::operator/(fraction const &other) const
{
    fraction result = *this;
    result /= other;
    return result;
}

fraction fraction::operator-() const
{
    fraction result = *this;
    result._numerator = big_int(0) - result._numerator;
    result.optimise();
    return result;
}

bool fraction::operator==(fraction const &other) const noexcept
{
    return _numerator * other._denominator == _denominator * other._numerator;
}

std::partial_ordering fraction::operator<=>(const fraction& other) const noexcept
{
    big_int lhs = _numerator * other._denominator;
    big_int rhs = other._numerator * _denominator;
    if (lhs < rhs)
        return std::partial_ordering::less;
    if (lhs > rhs)
        return std::partial_ordering::greater;
    return std::partial_ordering::equivalent;
}

std::ostream &operator<<(std::ostream &stream, fraction const &obj)
{
    stream << obj._numerator << '/' << obj._denominator;

    return stream;
}

std::istream &operator>>(std::istream &stream, fraction &obj)
{
    std::string input;
    stream >> input;

    size_t slash_position = input.find('/');
    if (slash_position != std::string::npos)
    {
        std::string num_str = input.substr(0, slash_position);
        std::string den_str = input.substr(slash_position + 1);

        obj._numerator = big_int(num_str);
        obj._denominator = big_int(den_str);
    }
    else
    {
        size_t dot_position = input.find('.');
        if (dot_position != std::string::npos)
        {
            std::string int_part = input.substr(0, dot_position);
            std::string frac_part = input.substr(dot_position + 1);

            big_int numerator = big_int(int_part + frac_part);
            big_int denominator("1");
            for (size_t i = 0; i < frac_part.length(); ++i)
                denominator *= big_int("10");
            
            obj._numerator = numerator;
            obj._denominator = denominator;
        }
        else{
            obj._numerator = big_int(input);
            obj._denominator = big_int(1);
        }
    }

    obj.optimise();
    return stream;
}

std::string fraction::to_string() const
{
    std::stringstream current_stream;   //TODO:FIXME:
    current_stream << _numerator << "/" << _denominator;
    return current_stream.str();
}
    //TRIGONOMETRY

fraction fraction::sin(fraction const &epsilon) const
{   
    fraction zero(0_bi,1_bi);
    if (epsilon <= zero)
        throw std::invalid_argument("Epsilon must be greater than 0");

    fraction one(1_bi, 1_bi);
    fraction two(2_bi, 1_bi);

    fraction x(*this);

    while (x > PI)
        x -= two * PI;
    while (x < PI)
        x += two * PI;

    fraction current = x;
    fraction sum = current;
    fraction n = one;

    do {
        fraction tmp = two * n;
        current *= -(x * x) / (tmp * (tmp + one));
        sum += current;
        n += one;
    }   while (current.abs() >= epsilon);
    return sum;
}

fraction fraction::cos(fraction const &epsilon) const
{
    fraction zero(0_bi, 1_bi);
    if (epsilon < zero)
        throw std::invalid_argument("Epsilon must be greater than 0");
    
    fraction one(1_bi, 1_bi);
    fraction two(2_bi, 1_bi);

    fraction x(*this);

    while (x > PI)
        x -= two * PI;
    while (x < PI)
        x += two * PI;

    fraction current = one;
    fraction sum = current;
    fraction n = one;
    do {
        fraction tmp = two * n;
        current *= -(x * x) / (tmp * (tmp - one));
        sum += current;
        n += one;
    }   while (current.abs() >= epsilon);
    return sum;
}

fraction fraction::tg(fraction const &epsilon) const
{
    fraction s = this->sin(epsilon);
    fraction c = this->cos(epsilon);

    if (c.abs() < epsilon)
        throw std::invalid_argument("tg(x): cos(x) too close to zero");

    return s / c;
}

fraction fraction::ctg(fraction const &epsilon) const
{
    fraction s = this->sin(epsilon);
    fraction c = this->cos(epsilon);

    if (s.abs() < epsilon)
        throw std::invalid_argument("ctg(x): sin(x) is too close to zero");
    
    return c / s;
}

fraction fraction::sec(fraction const &epsilon) const
{
    fraction c = this->cos(epsilon);
    if (c.abs() < epsilon)
        throw std::invalid_argument("sec(x): cos(x) is too close to zero");

    return fraction(1_bi, 1_bi) / c;
}

fraction fraction::cosec(fraction const &epsilon) const
{
    fraction s = this->sin(epsilon);

    if (s.abs() < epsilon)
        throw std::invalid_argument("cosec(x): sin(x) is too close to zero");

    return fraction(1_bi, 1_bi) / s;
}

fraction fraction::pow(size_t degree) const
{
    fraction zero(0_bi, 1_bi);
    fraction one(1_bi, 1_bi);

    if (*this == zero && degree == 0)
        throw std::invalid_argument("Attempt to raise zero in power zero");
    
    if (degree == 0)
        return one;

    fraction value = *this;
    fraction tmp_res = one;

    while (degree > 0)
    {
        if (degree & 1)
            tmp_res *= value;

        value *= value;
        degree >>= 1;
    }
    return tmp_res;
}

fraction fraction::root(size_t degree, fraction const &epsilon) const
{
    if (degree == 0)
        throw std::invalid_argument("Root degree cannot be zero");
    if (degree == 1)
        return *this;

    fraction zero(0_bi, 1_bi);
    fraction one(1_bi, 1_bi);
    fraction two(2_bi, 1_bi);
    fraction fraction_degree(big_int(std::to_string(degree)), 1_bi);
    fraction tmp_fraction = *this;

    int sign = 1;
    int l = 0;

    if (tmp_fraction <  zero)
    {
        if (degree & 1)
        {
            sign = -1;
            tmp_fraction *= -one;
        } else {
            throw std::invalid_argument("Cannot take odd degree root of negative number");
        }
    }

    fraction current = (one + tmp_fraction) / two;
    fraction next = current;

    fraction one_by_n(1_bi, big_int(std::to_string(degree)));
    fraction prev_degree = fraction_degree - one;

    do {
        current = next;     //Формула Ньютона
        next = one_by_n * (prev_degree * current + tmp_fraction / current.pow(degree - 1));
    }   while ((next - current).abs() >= epsilon);

    if (sign == -1)
        return -next;
    return next;
}

fraction fraction::log2(fraction const &epsilon) const
{
    //log b (x) == ln(x) / ln(b) => log 2 (x) == ln(x) / ln(2)
    fraction ln2 = fraction(2_bi, 1_bi).ln(epsilon);
    return this->ln(epsilon) / ln2;
}

fraction fraction::ln(fraction const &epsilon) const
{
    // для ln(1 + x) = x - x^2 / 2 + ...
    // если вызываем ln(5) и считаем ln(1 + 4) - очень медленно и неэффективно
    // ln(x) = kln(2) * ln(a), a э (0,5; 1,5]
    // y = (a-1)/(a+1)
    // ln(a) = 2 * (y + y^3/3 + y^5/5 + ...)

    fraction zero(0_bi, 1_bi);
    fraction one(1_bi, 1_bi);
    fraction two(2_bi, 1_bi);

    if (*this <= zero)
        throw std::invalid_argument("Cannot take log of a non-positive number");
    
    if (*this < one)
        return (-one) * (one / *this).ln(epsilon);

    if (*this > two)
    {
        fraction m = one;
        fraction dividend = *this;

        while ((dividend /= two) > one)
            m += one;

        return dividend.ln(epsilon) + m * two.ln(epsilon);
    }

    if (epsilon <= zero)
        throw std::invalid_argument("Epsilon must be greater than 0");

    fraction x = (*this - one) / (*this + one);
    fraction current = x;
    fraction sum = x;
    fraction x_squared = x * x;
    fraction n = one;

    while (current.abs() > epsilon)
    {
        n += two;
        current = current * x_squared;
        fraction term = current / n;
        sum += term;
    }
    return sum * two;
}

fraction fraction::lg(fraction const &epsilon) const
{
    // log 10 x = ln x / ln 10
    return ln(epsilon) / fraction(10_bi, 1_bi).ln(epsilon);
}

fraction fraction::arcsin(fraction const &epsilon = fraction(1_bi, 1000000_bi)) const
{       // Рекуррентная формула: a_{n+1} == a_n *  ((2 * n - 1) ^ 2 / ((2 * n) * (2 * n + 1))) * x ^ 2
    fraction zero(0_bi, 1_bi);
    fraction one(1_bi, 1_bi);
    fraction two(2_bi, 1_bi);

    if (epsilon <= zero)
        throw std::invalid_argument("Epsilon must be greater than 0");
    
    if (abs() > one)
        throw std::invalid_argument("Module of number must be not greater than 1");
    
    if (abs() > fraction(4_bi, 5_bi))
    {
        // if x > 0,8 : arcsin(x) = pi/2 − arcsin(root(1 − x ^ 2))
        return PI / two - (one - pow(2)).root(2, epsilon).arcsin(epsilon);
    }

    fraction const &x = *this;
    fraction current = x;
    fraction sum = current;
    fraction n = one;

    do {
        fraction tmp = two * n;
        fraction factor = (tmp - one).pow(2) / (tmp * (tmp + one));
        current *= factor * x * x;
        sum += current;
        n += one;
    }   while (current.abs() >= epsilon);

    return sum;
}

fraction fraction::arccos(fraction const &epsilon = fraction(1_bi, 1000000_bi)) const
{
    return HALF_PI - this->arcsin(epsilon);
}

fraction fraction::arctg(fraction const &epsilon = fraction(1_bi, 1000000_bi)) const
{   //arctg(x) == arcsin( x / root(1 + x ^ 2)
    return (*this / ((fraction(1_bi, 1_bi)) + pow(2)).root(2, epsilon)).arcsin(epsilon);

}

fraction fraction::arcctg(fraction const &epsilon = fraction(1_bi, 1000000_bi)) const
{       //arctg(x) == arccos (x / root (1 + x ^ 2))
        return (*this / ((fraction(1_bi, 1_bi)) + pow(2)).root(2, epsilon)).arccos(epsilon);
}

fraction fraction::arcsec(fraction const &epsilon = fraction(1_bi, 1000000_bi)) const
{       //arcsec(x) = arccos( 1 / x)
    return (fraction(1_bi, 1_bi) / *this).arccos(epsilon);
}

fraction fraction::arccosec(fraction const &epsilon = fraction(1_bi, 1000000_bi)) const
{       //arccosec(x) = arcsin(1 / x)
    return (fraction(1_bi, 1_bi) / *this).arcsin(epsilon);
}