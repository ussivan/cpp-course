#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <vector>
#include <cstddef>
#include <iosfwd>
#include <stdint.h>
#include "my_vector.cpp"

typedef uint32_t ui;
typedef uint64_t ull;

struct big_integer {

    big_integer();

    big_integer(big_integer const &other);

    big_integer(int a);

    explicit big_integer(std::string const &str);

    ~big_integer();

    big_integer &operator=(big_integer const &other);

    big_integer &operator+=(big_integer const &rhs);

    big_integer &operator+=(ui rhs);

    big_integer &operator-=(big_integer const &rhs);

    big_integer &operator*=(big_integer const &rhs);

    big_integer &operator/=(big_integer const &rhs);

    big_integer &operator%=(big_integer const &rhs);

    big_integer &operator&=(big_integer const &rhs);

    big_integer &operator|=(big_integer const &rhs);

    big_integer &operator^=(big_integer const &rhs);

    big_integer &operator<<=(int rhs);

    big_integer &operator>>=(int rhs);

    big_integer operator+() const;

    big_integer operator-() const;

    big_integer abs() const;

    ui digit(size_t i) const;

    big_integer& set_sign(ui new_sign);

    big_integer operator~() const;

    big_integer &operator++();

    big_integer operator++(int);

    big_integer &operator--();

    big_integer operator--(int);

    friend bool operator==(big_integer const &a, big_integer const &b);

    friend bool operator!=(big_integer const &a, big_integer const &b);

    friend bool operator<(big_integer const &a, big_integer const &b);

    friend bool operator>(big_integer const &a, big_integer const &b);

    friend bool operator<=(big_integer const &a, big_integer const &b);

    friend bool operator>=(big_integer const &a, big_integer const &b);

    friend big_integer operator*(big_integer a, big_integer const &b);

    friend big_integer operator*(big_integer a, ui b);

    friend big_integer operator+(big_integer a, big_integer const &b);

    friend big_integer operator-(big_integer a, big_integer const &b);

    friend big_integer operator/(big_integer a, big_integer const &b);

    friend big_integer operator%(big_integer a, big_integer const &b);

    friend big_integer operator&(big_integer a, big_integer const &b);

    friend big_integer operator|(big_integer a, big_integer const &b);

    friend big_integer operator^(big_integer a, big_integer const &b);

    friend big_integer operator<<(big_integer a, int b);

    friend big_integer operator>>(big_integer a, int b);

    friend std::string to_string(big_integer const &a);

    ui sign;
    my_vector<ui> digits;

    big_integer(ui a);

    void cut();

    ui get_digit(size_t index) const;

    friend std::pair<big_integer, big_integer> divmod(big_integer const &a, big_integer const &b);

    friend std::pair<big_integer, ui> divmod_by_digit(big_integer a, ui digit);

};

std::ostream &operator<<(std::ostream &s, big_integer const &a);

#endif // BIG_INTEGER_H