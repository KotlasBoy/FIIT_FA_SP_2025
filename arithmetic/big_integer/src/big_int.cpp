//
// Created by Des Caldnd on 5/27/2024.
//

#include "../include/big_int.h"
#include <ranges>
#include <exception>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>

bool is_zero(const auto& digits)            //simply check for zero equality
{
    return digits.empty() || digits.size() == 1 && digits.back() == 0;
}

void optimize (auto& digits)                //little-endian initial zeroes
{
    if (digits.size() <= 1) return;
    if (digits.back() != 0) return;

    while (digits.size() > 1 && digits.back() == 0)
    {
        digits.pop_back();
    }
}

std::strong_ordering modulo_comparison(const big_int &left, const big_int &right, size_t shift) noexcept {
    // Эффективная длина right с учетом смещения.
    size_t effective_right_size = right._digits.size() + shift;
    size_t left_size = left._digits.size();

    // Сначала сравним длины чисел.
    if (left_size < effective_right_size) {
        return std::strong_ordering::less;
    }
    if (left_size > effective_right_size) {
        return std::strong_ordering::greater;
    }

    // Если длины совпадают, сравниваем цифры по порядку, начиная с самого старшего разряда.
    // Индекс i пробегает от left_size-1 до 0.
    for (size_t i = left_size; i-- > 0;) {
        unsigned int left_digit = left._digits[i];
        // Для right, если i находится в области смещения, т.е. i < shift, то соответствующая цифра равна 0.
        // Иначе индекс в right равен (i - shift).
        unsigned int right_digit = (i < shift || (i - shift) >= right._digits.size()) ? 0 : right._digits[i - shift];
        if (left_digit < right_digit) {
            return std::strong_ordering::less;
        }
        if (left_digit > right_digit) {
            return std::strong_ordering::greater;
        }
    }

    // Если все разряды совпадают, числа равны.
    return std::strong_ordering::equal;
}

std::strong_ordering big_int::compare_with_sign(const big_int &left, const big_int &right, size_t shift) const noexcept {
    if (left._sign != right._sign) {
        // Если знаки разные, то положительное число больше отрицательного
        return left._sign ? std::strong_ordering::greater : std::strong_ordering::less;
    }
    if (left._sign == 0 && right._sign == 0) {
        auto result = modulo_comparison(left, right, shift);
        if (result == std::strong_ordering::less) return std::strong_ordering::greater;
        if (result == std::strong_ordering::greater) return std::strong_ordering::less;
        return std::strong_ordering::equal;
    }

    return modulo_comparison(left, right, shift);
}

std::strong_ordering big_int::operator<=>(const big_int &other) const noexcept
{
    return compare_with_sign(*this, other, 0 );
}

big_int::operator bool() const noexcept
{
    return !is_zero(_digits);
}

big_int &big_int::operator++() &   
{
    return this->operator+=(1);
}

big_int big_int::operator++(int)       
{
    auto tmp = *this;
    this->operator+=(1);
    return tmp;

}

big_int &big_int::operator--() &    
{
    return this->operator-=(1);         
}

big_int big_int::operator--(int)   
{
    auto tmp = *this;
    this->operator-=(1);
    return tmp;
}

big_int &big_int::operator+=(const big_int &other) &
{
    return plus_assign(other, 0);
}

big_int &big_int::operator-=(const big_int &other) &
{
    return minus_assign(other, 0);
}

big_int big_int::operator+(const big_int &other) const
{
    big_int lhs = *this;
    return lhs += other;
}

big_int big_int::operator-(const big_int &other) const
{
    big_int rhs = *this;
    return rhs -= other;
}

big_int big_int::operator-() const {
    big_int tmp = *this;
    tmp._sign = !_sign;
    return tmp;
}

big_int big_int::operator*(const big_int &other) const
{
    big_int lhs = *this;
    return lhs *= other;
}

big_int big_int::operator/(const big_int &other) const
{
    big_int lhs = *this;
    return lhs /= other;
}

big_int big_int::operator%(const big_int &other) const
{
    big_int lhs = *this;
    return lhs %= other;
}

big_int big_int::operator&(const big_int &other) const
{
    big_int lhs = *this;
    return lhs &= other;
}

big_int big_int::operator|(const big_int &other) const
{
    big_int lhs = *this;
    return lhs |= other;
}

big_int big_int::operator^(const big_int &other) const
{
    big_int lhs = *this;
    return lhs ^= other;
}

big_int big_int::operator<<(size_t shift) const
{
    big_int lhs = *this;
    return lhs<<=shift;
}

big_int big_int::operator>>(size_t shift) const
{
    big_int lhs = *this;
    return lhs>>=shift;
}

big_int &big_int::operator%=(const big_int &other) &
{
    return operator-= ((*this / other) * other);
}

big_int big_int::operator~() const
{
    big_int result = *this;
    result._sign = !result._sign;
    for (auto &digit : result._digits)
        digit = ~digit;
    
    optimize(result._digits);
    return result;
}
                                                                    
big_int &big_int::operator&=(const big_int &other) &
{
    if (!_sign && other._sign)
        _sign = true;

    size_t max_size = std::max(_digits.size(), other._digits.size());
    _digits.resize(max_size, 0);
    for (size_t i = 0; i < max_size; ++i)
        _digits[i] &= (i < other._digits.size()) ? other._digits[i] : 0;

    optimize(_digits);
    return *this;
}

big_int &big_int::operator|=(const big_int &other) &
{
    if (!_sign && other._sign)
        _sign = true;

    size_t max_size = std::max(_digits.size(), other._digits.size());
    _digits.resize(max_size, 0);
    for (size_t i = 0; i < max_size; ++i)
    {
        _digits[i] |= (i < other._digits.size()) ? other._digits[i] : 0;
    }

    optimize(_digits);
    return *this;
}

big_int &big_int::operator^=(const big_int &other) &
{
    if (!_sign && other._sign)
        _sign = true;

    size_t max_size = std::max(_digits.size(), other._digits.size());
    _digits.resize(max_size, 0);

    for (size_t i = 0; i < max_size; ++i)
    {
        _digits[i] ^= (i < other._digits.size()) ? other._digits[i] : 0;
    }

    optimize(_digits);
    return *this;
}

big_int &big_int::operator<<=(size_t shift) &
{
    if (shift == 0 || is_zero(_digits))
        return *this;

    const size_t bits_in_digit = std::numeric_limits<unsigned int>::digits;
    const size_t full_digits = shift / bits_in_digit;             // полные цифры для сдвига
    const size_t bit_offset = shift % bits_in_digit;               //остаточный сдвиг

    if (full_digits != 0)
        _digits.insert(_digits.begin(), full_digits, 0);

    if (bit_offset != 0)
    {
        unsigned int carry = 0;
        for (auto &d : _digits)
        {
            unsigned int tmp_digit = d;
            d = (tmp_digit << bit_offset) | carry;
            carry = tmp_digit >> (bits_in_digit - bit_offset);
        }
        if (carry != 0)
            _digits.push_back(carry);
    }

    optimize(_digits);
    return *this;
}

big_int &big_int::operator>>=(size_t shift) &
{
    if (shift == 0 || is_zero(_digits))
        return *this;

    const size_t bits_in_digit = std::numeric_limits<unsigned int>::digits;
    const size_t full_digits = shift / bits_in_digit;
    const size_t bit_offset = shift % bits_in_digit;

    if (full_digits != 0)
    {
        if (full_digits >= _digits.size())
        {
            _digits = {0};
            _sign = true;
            return *this;
        }
        _digits.erase(_digits.begin(), _digits.begin() + full_digits);
    }

    if (bit_offset != 0)
    {
        unsigned int carry = 0;
        for (auto it = _digits.begin(); it != _digits.end(); ++it)
        {
            unsigned int tmp = *it;
            *it = (tmp >> bit_offset) | carry;
            carry = tmp << (bits_in_digit - bit_offset);
        }
    }

    optimize(_digits);
    if (is_zero(_digits))
        _sign = true;

    return *this;
}

void big_int::plus_operation_without_sign(big_int &left, const big_int &right, size_t shift) noexcept {
    /*
     * Пример:
     * {1 0 3} + {2, 1} shift = 2
     * result = {1 0 3 0}
     *         +{0 0 2 1} <- сдвинули на 2
     *          {1 0 5 1}
     *
     */

    // Убедимся, что в результате достаточно места для разрядов right со сдвигом
    if (left._digits.size() < right._digits.size() + shift) {
        left._digits.resize(right._digits.size() + shift, 0);
    }

    unsigned int carry = 0;
    size_t i = 0;
    // Проходим по цифрам числа right
    constexpr unsigned int half_bits = sizeof(unsigned int) * 4;
    constexpr unsigned int low_mask = __detail::generate_half_mask();
    constexpr unsigned int high_mask = low_mask << half_bits;

    for (; i < right._digits.size(); ++i) {
        size_t j = i + shift; // Смещаем индекс на shift
        // Если нет переноса разряда

        const unsigned int left_low = left._digits[j] & low_mask;
        const unsigned int left_high = left._digits[j] >> half_bits;
        const unsigned int right_low = right._digits[j] & low_mask;
        const unsigned int right_high = right._digits[j] >> half_bits;

        const unsigned int low_result = left_low + right_low + carry;
        const unsigned int high_result = (low_result >> half_bits) + left_high + right_high;

        left._digits[j] = (high_result << half_bits) | (low_result & low_mask);
        carry = high_result >> half_bits;
    }

    // Продолжаем обработку остаточного переноса, если он ещё остался
    size_t j = i + shift;
    while (carry != 0 && j < left._digits.size()) {
        const unsigned int left_low = left._digits[j] & low_mask;
        const unsigned int left_high = left._digits[j] >> half_bits;

        const unsigned int low_result = left_low + carry;
        const unsigned int high_result = (low_result >> half_bits) + left_high;

        left._digits[j] = (high_result << half_bits) | (low_result & low_mask);
        carry = high_result >> half_bits;

        ++j;
    }
    if (carry != 0) {
        left._digits.push_back(1);
    }
}

void big_int::minus_operation_without_sign(big_int &left, const big_int &right, size_t shift) {
    // left >= right
    if (modulo_comparison(left, right, shift) == std::strong_ordering::less) {
        throw std::invalid_argument("Subtraction cannot be performed: left is smaller than right");
    }

    unsigned int carry = 0;
    size_t i = 0;
    // Проходим по цифрам числа right
    for (; i < right._digits.size(); ++i) {
        size_t j = i + shift; // Смещаем индекс на shift

        unsigned int subtrahend = right._digits[i];
        unsigned int total_subtrahend = subtrahend + carry;

        // Проверяем было ли переполнение при сложении subtrahend + carry
        bool overflow_add = (total_subtrahend < subtrahend);

        if (!overflow_add && left._digits[j] >= total_subtrahend) {
            // Обычное вычитание
            left._digits[j] -= total_subtrahend;
            carry = 0;
        } else {
            // Случай с переносом
            if (left._digits[j] == 0 && overflow_add) {
                // Особый случай: subtrahend + carry дало переполнение (0)
                left._digits[j] = std::numeric_limits<unsigned int>::max();
            } else {
                // Стандартный случай переноса
                left._digits[j] = (std::numeric_limits<unsigned int>::max() - subtrahend + 1)
                                + left._digits[j] - carry;
            }
            carry = 1;
        }
    }

    // Продолжаем обработку остаточного переноса
    size_t j = i + shift;
    while (carry != 0 && j < left._digits.size()) {
        if (left._digits[j] >= carry) {
            left._digits[j] -= carry;
            carry = 0;
        } else {
            left._digits[j] = std::numeric_limits<unsigned int>::max();
            carry = 1;
        }
        ++j;
    }

    optimize(left._digits);
}


big_int &big_int::plus_assign(const big_int &other, size_t shift) &
{
    if (this->_sign == other._sign)
    {
        plus_operation_without_sign(*this, other, shift);
        return *this;
    }
    // знаки разные
    // 1 случай this >= other
    // Знак остается от this
    auto result = modulo_comparison(*this, other);
    if (result == std::strong_ordering::greater || result == std::strong_ordering::equal)
    {
        minus_operation_without_sign(*this, other, shift);
        return *this;
    }

    // 2 случай this < other
    big_int copy = other;
    minus_operation_without_sign(copy, *this, shift);
    *this = copy;
    return *this;

}

big_int &big_int::minus_assign(const big_int &other, size_t shift) &
{
    return plus_assign(-other, shift);
}

big_int &big_int::operator*=(const big_int &other) &
{
    return multiply_assign(other, decide_mult(other._digits.size()));
}

big_int &big_int::operator/=(const big_int &other) &
{
    return divide_assign(other, decide_div(other._digits.size()));
}

std::string big_int::to_string() const
{
    if(is_zero(_digits))
        return "0";

    std::string res;
    big_int tmp = *this;
    tmp._sign = true;
    while (tmp)
    {
        auto val = tmp % 10;
        res += ('0' + val._digits[0]);
        tmp /= 10;
    }
    if (!_sign)
        res += '-';

    std::reverse(res.begin(), res.end());
    return res;
}

std::ostream &operator<<(std::ostream &stream, const big_int &value)        //TODO:FIXME:
{
    stream << value.to_string();
    return stream;
}

std::istream &operator>>(std::istream &stream, big_int &value)      //TODO:FIXME:
{
    std::string val;
    stream >> val;
    value = big_int(val);
    return stream;                      
}

bool big_int::operator==(const big_int &other) const noexcept
{
    return std::strong_ordering::equal == (*this <=> other);
}

big_int::big_int(const std::vector<unsigned int, pp_allocator<unsigned int>> &digits, bool sign) : _digits(digits), _sign(sign)
{
    if (_digits.empty())
        _digits.push_back(0);
    optimize(_digits);
}

big_int::big_int(std::vector<unsigned int, pp_allocator<unsigned int>> &&digits, bool sign) noexcept : _digits(std::move(digits)), _sign(sign)
{
    if (_digits.empty())
        _digits.push_back(0);
    optimize(_digits);
}

big_int::big_int(const std::string &num, unsigned int radix, pp_allocator<unsigned int> alloc)
{
    if (radix > 36 || radix < 2)
        throw std::invalid_argument("Radix must be in interval [2, 36], but is equal to: " + std::to_string(radix));

    if (num.empty())
    {
        _digits.push_back(0);
        return;
    }

    std::string u_num = num;
    bool is_negative = u_num[0] == '-';
    if (is_negative || u_num[0] == '+')
    {
        u_num = u_num.substr(1);
    }

    if (u_num.empty())
    {
        _digits.push_back(0);
        return;
    }

    while (u_num.size() > 1 && u_num[0] == '0')
        u_num = u_num.substr(1);

    for (char c : u_num)
    {
        unsigned int solo_digit;
        if (c >= '0' && c <= '9')
            solo_digit = c - '0';
        else if (c >= 'A' && c <= 'Z')
            solo_digit = 10 + c - 'A';
        else if (c >= 'a' && c <= 'z')
            solo_digit = 10 + c - 'a';
        else
            throw std::invalid_argument("Invalid character in number string");
        
        if (solo_digit >= radix)
            throw std::invalid_argument("Digit exceeds radix");
        
        *this *= radix;
        *this += big_int(static_cast<long long>(solo_digit), alloc);
    }

    _sign = !is_negative;
    if (is_zero(_digits))
        _sign = true;
}

big_int::big_int(pp_allocator<unsigned int> allocator) : _digits({0}, allocator), _sign(true)     //TODO:FIXME:
{
    _digits.push_back(0);
}

big_int &big_int::multiply_assign(const big_int &other, big_int::multiplication_rule rule) &        //TODO:FIXME:
{
    if (is_zero(_digits))
        return *this;

    if (rule == multiplication_rule::trivial) {
        *this = std::move(multiply_table(*this, other));
    }
    if (rule == multiplication_rule::Karatsuba) {
        *this = std::move(multiply_karatsuba(*this, other));
    }
}

big_int &big_int::divide_assign(const big_int &other, big_int::division_rule rule) &                //TODO:FIXME:
{
   if (rule == division_rule::trivial) {
        *this = std::move(divide_table(*this, other));
    }
    return *this;

}

big_int divide_table(const big_int &numerator, const big_int &denominator) {
    if (is_zero(denominator._digits)) {
        throw std::invalid_argument("Zero division");
    }
    big_int abs_dividend = numerator;
    big_int abs_divisor = denominator;
    abs_dividend._sign = true;
    abs_divisor._sign = true;

    big_int quotient; // частное
    big_int remainder; // остаток
    remainder._digits.push_back(0);

    for (long long pos = static_cast<long long>(abs_dividend._digits.size()) - 1; pos >= 0; --pos) {
        // вставляем в остаток самую старшую цифру делимого
        remainder._digits.insert(remainder._digits.begin(), abs_dividend._digits[pos]);
        optimize(remainder._digits);

        // Находим цифру частного:
        // такую что (digit * absDivisor) <= remainder.
        big_int digit = binary_search_quotient(remainder, abs_divisor);

        // Записываем найденную цифру в частное.
        quotient._digits.insert(quotient._digits.begin(), digit._digits[0]);

        // Вычитаем произведение делителя на найденную цифру из остатка.
        big_int prod = abs_divisor * digit;
        remainder = remainder - prod;
        optimize(remainder._digits);
    }
    optimize(quotient._digits);
    // Определяем знак частного: оно положительно, если делимое и делитель имеют одинаковый знак.
    quotient._sign = (numerator._sign == denominator._sign);
    return quotient;
}

big_int binary_search_quotient(const big_int &numerator, const big_int &denominator) {
    // Бинарный двоичный поиск
    big_int result;
    result._digits.push_back(0);
    unsigned int &closest = result._digits[0];
    for (int i = sizeof(unsigned int) * 8 - 1; i >= 0; --i) {
        const unsigned int temp = closest;
        closest |= 1 << i;
        big_int multiplied = result * denominator;

        auto comp = modulo_comparison(multiplied, numerator);
        if (comp == std::strong_ordering::equal) {
            return result;
        } else if (comp == std::strong_ordering::less) {
            continue;
        } else {
            result._digits[0] = temp;
        }
    }
    return result;
}

big_int &big_int::modulo_assign(const big_int &other, big_int::division_rule rule) &
{
    if (is_zero(other._digits))
        throw std::logic_error("Division by zero");

    if (is_zero(_digits))
        return *this;

    const bool dividend_sign = _sign;
    _sign = true;
    big_int abs_other = other;
    abs_other._sign = true;

    //quoitient -- частное
    big_int quotient = *this / abs_other;
    *this -= quotient * abs_other;

    _sign = dividend_sign;

    if (!_sign && !is_zero(_digits))        // dividend was negative  
        *this -= abs_other;

    optimize(_digits);
    return *this;
}

big_int::multiplication_rule big_int::decide_mult(size_t rhs) const noexcept
{
    return rhs > 64 ? big_int::multiplication_rule::Karatsuba : big_int::multiplication_rule::trivial;
}

big_int big_int::get_lower_half(size_t half_size) const
{
    big_int result;
    size_t size = std::min(half_size, _digits.size());
    result._digits.assign(_digits.begin(), _digits.begin() + size);
    result._sign = true;
    return result; 
}

big_int big_int::get_upper_half(size_t half_size) const
{
    big_int result;
    if (_digits.size() > half_size)
    {
        result._digits.assign(_digits.begin() + half_size, _digits.end());
    }
    result._sign = true;
    return result;
}

big_int multiply_table(const big_int &left, const big_int &right) noexcept {
    const size_t new_size = left._digits.size() + right._digits.size();
    std::vector<unsigned int, pp_allocator<unsigned int>> result(new_size, 0, left._digits.get_allocator());

    for (size_t i = 0; i < left._digits.size(); ++i) {
        uint64_t carry = 0;
        const uint64_t a = left._digits[i];

        for (size_t j = 0; j < right._digits.size(); ++j) {
            uint64_t product = a * right._digits[j] + result[i+j] + carry;
            result[i+j] = static_cast<unsigned int>(product % BASE);
            carry = product / BASE;
        }

        for (size_t k = i + right._digits.size(); carry > 0 && k < new_size; ++k) {
            uint64_t sum = result[k] + carry;
            result[k] = sum % BASE;
            carry = sum / BASE;
        }
    }

    big_int return_value(result, left._sign == right._sign);
    optimize(return_value._digits);
    return return_value;
}

big_int multiply_karatsuba(const big_int &left, const big_int &right)     //TODO:FIXME:      
{
    if (is_zero(left._digits) || is_zero(right._digits)) {
        return big_int(0);
    }
    size_t m = std::max(left._digits.size(), right._digits.size());
    if (m == 1) {
        return multiply_table(left, right);
    }

    size_t half = (m + 1) / 2;

    // Лямбда для безопасного разбиения
    auto split = [](const big_int& num, size_t pos) -> std::pair<big_int, big_int> {
        pos = std::min(pos, num._digits.size());
        return {
                big_int(std::vector<unsigned int>(num._digits.begin(), num._digits.begin() + pos), num._sign),
                big_int(std::vector<unsigned int>(num._digits.begin() + pos, num._digits.end()), num._sign)
        };
    };

    auto [left_low, left_high] = split(left, half);
    auto [right_low, right_high] = split(right, half);

    // Рекурсивные вызовы
    big_int z0 = multiply_karatsuba(left_low, right_low);
    big_int z2 = multiply_karatsuba(left_high, right_high);

    // Вычисляем z1 = (left_low + left_high) * (right_low + right_high) - z0 - z2
    big_int sum_left = left_low + left_high;
    big_int sum_right = right_low + right_high;
    big_int z1 = multiply_karatsuba(sum_left, sum_right) - z0 - z2;

    // Сборка результата
    big_int result = z0;
    result += (z1 << (half * sizeof(unsigned int) * 8));
    result += (z2 << (2 * half * sizeof(unsigned int) * 8));

    result._sign = (left._sign == right._sign);
    optimize(result._digits);
    return result;
}
big_int operator""_bi(unsigned long long n)     //TODO:FIXME:
{
    return {n};
}