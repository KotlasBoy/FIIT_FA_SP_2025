//
// Created by Des Caldnd on 5/27/2024.
//

#ifndef MP_OS_BIG_INT_H
#define MP_OS_BIG_INT_H

#include <vector>                       // храним цифры
#include <utility>                      // юзаем пары
#include <iostream>                     // ввод/вывод
#include <concepts>                     // концепты с++20   //TODO:FIXME:
#include <pp_allocator.h>               // custom allocator //TODO:FIXME:
#include <not_implemented.h>            

inline constexpr unsigned long long BASE = 1ULL << (8 * sizeof(unsigned int));      //a0 * BASE^0 + a1 * BASE^1 + a2 * BASE^2 + ...

namespace __detail
{
    constexpr unsigned int generate_half_mask()                     // 00001111
    {
        unsigned int res = 0;
        for(size_t i = 0; i < sizeof(unsigned int) * 4; ++i)        
            res |= (1u << i);

        return res;
    }

    //constexpr evaluate during compilet time. Optimize perfomance 
    constexpr size_t nearest_greater_power_of_2(size_t size) noexcept
    {
        int ones_counter = 0, index = 0;

        constexpr const size_t o = 1;

        for (int i = sizeof(size_t) * 8 - 1; i >= 0; --i)
        {
            if (size & (o << i))
            {   
                if (ones_counter == 0)                              // первый единичный бит
                    index = i;
                ++ones_counter;
            }
        }

        return ones_counter <= 1 ? (1u << index) : (1u << (index + 1));     //
    }
}

class big_int
{
    // Call optimise after every operation!!!
    bool _sign; // 1 +  0 -       
    std::vector<unsigned int, pp_allocator<unsigned int>> _digits;       

public:

    enum class multiplication_rule          //3 different algorithms to do
    {
        trivial,
        Karatsuba,
        SchonhageStrassen
    };

    enum class division_rule                //3 different algorithms to do
    {
        trivial,
        Newton,
        BurnikelZiegler
    };

private:

    // Decides type of mult/div that depends on size of lhs and rhs         // left-hand side && right-hand side
    multiplication_rule decide_mult(size_t rhs) const noexcept;
    division_rule decide_div(size_t rhs) const noexcept;

public:

    using value_type = unsigned int;

    template<class alloc> // Шаблонный конструктор из вектора в big_int                 
    explicit big_int(const std::vector<unsigned int, alloc> &digits, bool sign = true, pp_allocator<unsigned int> allocator = pp_allocator<unsigned int>());

                        // Конструктор из вектора 
    explicit big_int(const std::vector<unsigned int, pp_allocator<unsigned int>> &digits, bool sign = true);
                        // Move конструктор
    explicit big_int(std::vector<unsigned int, pp_allocator<unsigned int>> &&digits, bool sign = true) noexcept;
                        // Конструктор из строки
    explicit big_int(const std::string& num, unsigned int radix = 10, pp_allocator<unsigned int> = pp_allocator<unsigned int>());

    template<std::integral Num>             //Шаблонный конструктор из интегральных типов  (bool, char, int)  // concept    
    big_int(Num d, pp_allocator<unsigned int> = pp_allocator<unsigned int>());
                        // Конструктор по умолчанию
    big_int(pp_allocator<unsigned int> = pp_allocator<unsigned int>());

    explicit operator bool() const noexcept; //if 0 -- false , else true           //cast to bool

    big_int& operator++() &;        // lvalue prefix only
    big_int operator++(int);        // postfix

    big_int& operator--() &;
    big_int operator--(int);

    big_int& operator+=(const big_int& other) &;        // сложение с параметром сдвига
    /** Shift will be needed for multiplication implementation
     *  @example Shift = 0: 111 + 222 = 333
     *  @example Shift = 1: 111 + 222 = 2331
     */
    big_int& plus_assign(const big_int& other, size_t shift = 0) &;

    void big_int::plus_operation_without_sign(big_int &left, const big_int &right, size_t shift) noexcept;
    void big_int::minus_operation_without_sign(big_int &left, const big_int &right, size_t shift);

    friend std::strong_ordering modulo_comparison(const big_int &left, const big_int &right, size_t shift = 0) noexcept;
    std::strong_ordering big_int::compare_with_sign(const big_int &left, const big_int &right, size_t shift) const noexcept;

    big_int& operator-=(const big_int& other) &;        // вычитание с параметром сдвига
    big_int& minus_assign(const big_int& other, size_t shift = 0) &;

    big_int& operator*=(const big_int& other) &;    // decide_mult() and calls multiply_assign()
    big_int& multiply_assign(const big_int& other, multiplication_rule rule = multiplication_rule::trivial) &;  // по умолчанию trivial

    big_int& operator/=(const big_int& other) &;
    big_int& divide_assign(const big_int& other, division_rule rule = division_rule::trivial) &;

    friend big_int divide_table(const big_int &numerator, const big_int &denominator);
    friend big_int binary_search_quotient(const big_int &numerator, const big_int &denominator);

    big_int& operator%=(const big_int& other) &;
    big_int& modulo_assign(const big_int& other, division_rule rule = division_rule::trivial) &;

    big_int operator+(const big_int& other) const;
    big_int operator-(const big_int& other) const;
    big_int operator-() const;
    big_int operator*(const big_int& other) const;
    big_int operator/(const big_int& other) const;
    big_int operator%(const big_int& other) const;

    std::strong_ordering operator<=>(const big_int& other) const noexcept;      // spaceship operator       // copmarison
    bool operator==(const big_int& other) const noexcept;

    big_int& operator<<=(size_t shift) &;           //bit shifts
    big_int& operator>>=(size_t shift) &;

    big_int operator<<(size_t shift) const;
    big_int operator>>(size_t shift) const;

    big_int operator~() const;                      //bit operators

    big_int& operator&=(const big_int& other) &;
    big_int& operator|=(const big_int& other) &;
    big_int& operator^=(const big_int& other) &;

    big_int operator&(const big_int& other) const;
    big_int operator|(const big_int& other) const;
    big_int operator^(const big_int& other) const;

    friend std::ostream &operator<<(std::ostream &stream, big_int const &value);    //операторы ввода вывода

    friend std::istream &operator>>(std::istream &stream, big_int &value);

    friend big_int multiply_table(const big_int &left, const big_int &right) noexcept;
    friend big_int multiply_karatsuba(const big_int &left, const big_int &right);

    std::string to_string() const;                                                  // преобразование в строку

    big_int get_lower_half(size_t half_size) const;
    big_int get_upper_half(size_t half_size) const;
};

template<class alloc>                                
big_int::big_int(const std::vector<unsigned int, alloc> &digits, bool sign, pp_allocator<unsigned int> allocator) : _digits(digits.begin(), digits.end(), allocator), _sign(sign)
{
    if (_digits.empty())
        _digits.push_back(0);

    while (_digits.size() > 1 && _digits.back() == 0)       // little-endian, remove initial 00000
        _digits.pop_bak();
}

template<std::integral Num>
big_int::big_int(Num d, pp_allocator<unsigned int>) : _sign(d >= 0)
{
    auto abs_d = d >= 0 ? d : -d;
    if (abs_d == 0)
        _digits.push_back(0);
    else
    {
        unsigned long long BASE = 1ULL << (sizeof(unsigned long long) * 8); // TODO:FIXME:
        while (abs_d > 0)
        {
            _digits.push_back(abs_d % BASE);
            abs_d /= BASE;
        }
    }

    while (_digits.size() > 0 && _digits.back() == 0)
        _digits.pop_back();
}

big_int operator""_bi(unsigned long long n);    // 156_bi

#endif //MP_OS_BIG_INT_H
