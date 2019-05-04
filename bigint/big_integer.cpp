#include "big_integer.h"
#include <algorithm>

#include <cstring>

const ui max_ui = std::numeric_limits<ui>::max();
const big_integer ZERO = big_integer(0);
const ui MAX_2 = static_cast<ui>(std::numeric_limits<ui>::digits);

big_integer::big_integer() : sign(0), digits({0}) {}

big_integer::big_integer(big_integer const &other) = default;

ui big_integer::get_digit(size_t index) const {
    return (index < digits.size() ? digits[index] : sign);
}

big_integer::big_integer(int a) {
    sign = a >= 0 ? 0 : max_ui;
    digits.push_back(static_cast<ui>(a));
}

big_integer::big_integer(std::string const &str) : big_integer() {
    ui result_sign = 0;
    size_t pos = 0;
    if (str[0] == '-') {
        result_sign = max_ui;
        pos++;
    }
    for (; pos < str.size(); pos++) {
        *this *= 10;
        *this += (str[pos] - '0');
    }
    if (result_sign > 0) {
        *this = -*this;
    }
}

big_integer::big_integer(ui a) {
    sign = 0;
    digits.push_back(a);
}

big_integer::~big_integer() = default;

big_integer &big_integer::operator=(big_integer const &other) = default;

void big_integer::cut() {
    while (digits.size() > 1 && digits.back() == sign) {
        digits.pop_back();
    }
    if (digits[0] == 0 && digits.size() == 1) {
        sign = 0;
    }
}

big_integer &big_integer::operator+=(ui rhs) {
    ui carry = 0;
    digits.resize(digits.size() + 2, sign);
    for (size_t i = 0; i < digits.size(); i++) {
        ui digit = i > 0 ? 0 : rhs;
        ull sum = 1ull * digits[i] + carry + digit;
        carry = 0;
        if (sum > max_ui) {
            carry = 1;
            sum -= max_ui + 1;
        }
        digits[i] = static_cast<ui>(sum);
    }
    sign = digits.back();
    cut();
    return *this;
}

big_integer &big_integer::operator+=(big_integer const &rhs) {
    ui carry = 0;
    digits.resize(std::max(digits.size(), rhs.digits.size()) + 2, sign);
    for (size_t i = 0; i < digits.size(); i++) {
        ui digit = i >= rhs.digits.size() ? rhs.sign : rhs.digits[i];
        ull sum = 1ull * digits[i] + carry + digit;
        carry = 0;
        if (sum > max_ui) {
            carry = 1;
            sum -= max_ui + 1;
        }
        digits[i] = static_cast<ui>(sum);
    }
    sign = digits.back();
    cut();
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    return *this += -rhs;
}

big_integer big_integer::abs() const {
    big_integer a = *this;
    return sign > 0 ? -a : a;
}

big_integer operator*(big_integer a, ui b) {
    ui carry = 0;
    for (size_t i = 0; i < a.digits.size(); i++) {
        ull sum = 1ull * a.digits[i] * b + carry;
        carry = static_cast<ui>(sum / (1ull + max_ui));
        a.digits[i] = static_cast<ui>(sum % (1ull + max_ui));
    }
    if (carry) {
        a.digits.push_back(carry);
    }
    a.cut();
    return a;
}


big_integer &big_integer::operator*=(big_integer const &rhs) {
    ui final_sign = sign ^rhs.sign;
    big_integer a = abs();
    big_integer b = rhs.abs();
    if (a > b) {
        std::swap(a.digits, b.digits);
    }
    big_integer res;
    for (size_t i = 0; i < a.digits.size(); i++) {
        res += b * a.digits[i];
        b.digits.insert(b.digits.begin(), 0);
    }
    if (final_sign) {
        res = -res;
    }
    cut();
    *this = res;
    return *this;
}

ui big_integer::digit(size_t i) const {
    return i < digits.size() ? digits[i] : sign;
}

big_integer &big_integer::set_sign(ui new_sign) {
    return sign == new_sign ? *this : *this = -*this;
}

std::pair<big_integer, ui> divmod_by_digit(big_integer a, ui digit) {
    ui carry = 0;
    for (size_t i = a.digits.size(); i-- > 0;) {
        ull cur = a.digits[i] + carry * (1ull + max_ui);
        a.digits[i] = static_cast<ui>(cur / digit);
        carry = static_cast<ui>(cur % digit);
    }
    a.cut();
    return {a, carry};
}

std::pair<big_integer, big_integer> divmod(big_integer const &a, big_integer const &b) {
    ui q_sign = a.sign ^b.sign;
    ui r_sign = a.sign;
    big_integer a_abs = a.abs();
    big_integer b_abs = b.abs();
    ui norm = static_cast<ui>((1ull + max_ui) / (1ull + b_abs.digits.back()));
    a_abs *= norm;
    b_abs *= norm;
    big_integer q, r;
    q.digits.resize(a_abs.digits.size());
    for (size_t i = a_abs.digits.size(); i-- > 0;) {
        r.digits.insert(r.digits.begin(), 0);
        r += a_abs.get_digit(i);
        ui s1 = r.get_digit(b_abs.digits.size());
        ui s2 = r.get_digit(b_abs.digits.size() - 1);
        ui d = static_cast<ui>(((1ull + max_ui) * s1 + s2) / b_abs.digits.back());
        r -= b_abs * d;
        while (r < 0) {
            r += b_abs;
            --d;
        }
        q.digits[i] = d;
    }
    if (q_sign) {
        q = -q;
    }
    q.cut();
    r = divmod_by_digit(r, norm).first;
    if (r_sign) {
        r = -r;
    }
    r.cut();
    return {q, r};
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    return *this = divmod(*this, rhs).first;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    return *this = divmod(*this, rhs).second;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    digits.resize(std::max(digits.size(), rhs.digits.size()), sign);
    for (size_t i = 0; i < digits.size(); i++) {
        digits[i] = digits[i] & (i >= rhs.digits.size() ? rhs.sign : rhs.digits[i]);
    }
    sign = sign & rhs.sign;
    cut();
    return *this;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    digits.resize(std::max(digits.size(), rhs.digits.size()), sign);
    for (size_t i = 0; i < digits.size(); i++) {
        digits[i] = digits[i] | (i >= rhs.digits.size() ? rhs.sign : rhs.digits[i]);
    }
    sign = sign | rhs.sign;
    cut();
    return *this;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    digits.resize(std::max(digits.size(), rhs.digits.size()), sign);
    for (size_t i = 0; i < digits.size(); i++) {
        digits[i] = digits[i] ^ (i >= rhs.digits.size() ? rhs.sign : rhs.digits[i]);
    }
    sign = sign ^ rhs.sign;
    cut();
    return *this;
}

big_integer &big_integer::operator<<=(int rhs) {
    if (rhs < 0) {
        return *this >>= (-rhs);
    }
    if (rhs == 0) {
        return *this;
    }
    digits.insert(digits.begin(), rhs / MAX_2, 0);
    digits.push_back(sign);
    ui carry = 0;
    ui left = rhs % MAX_2;
    for (ui &digit : digits) {
        ui old_digit = digit;
        digit = ((digit << left) | carry);
        carry = (old_digit >> (MAX_2 - left));
    }
    cut();
    return *this;
}

big_integer &big_integer::operator>>=(int rhs) {
    if (rhs < 0) {
        return *this <<= (-rhs);
    }
    digits.erase(digits.begin(), digits.begin() + std::min<size_t>(digits.size(), rhs / MAX_2));
    ui carry = sign;
    unsigned left = rhs % MAX_2;
    for (size_t i = digits.size(); i-- > 0;) {
        ui old_digit = digits[i];
        digits[i] = ((carry << (MAX_2 - left)) | (digits[i] >> left));
        carry = (old_digit & ((1u << left) - 1));
    }
    cut();
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    return ~*this + 1;
}

big_integer big_integer::operator~() const {
    big_integer r = *this;
    r.sign = ~r.sign;
    for (size_t i = 0; i < r.digits.size(); i++) {
        r.digits[i] = ~r.digits[i];
    }
    return r;
}

big_integer &big_integer::operator++() {
    *this += 1;
    return *this;
}

big_integer big_integer::operator++(int) {
    big_integer r = *this;
    ++*this;
    return r;
}

big_integer &big_integer::operator--() {
    *this -= 1;
    return *this;
}

big_integer big_integer::operator--(int) {
    big_integer r = *this;
    --*this;
    return r;
}

big_integer operator+(big_integer a, big_integer const &b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b) {
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const &b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

bool operator==(big_integer const &a, big_integer const &b) {
    return (a >= b) && (a <= b);
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.sign != b.sign) {
        return a.sign > b.sign;
    }
    if (a.digits.size() != b.digits.size()) {
        return a.sign ? a.digits.size() > b.digits.size() : a.digits.size() < b.digits.size();
    }
    for (size_t i = a.digits.size(); i-- > 0;) {
        if (a.digits[i] != b.digits[i]) {
            return a.digits[i] < b.digits[i];
        }
    }
    return false;
}

bool operator>(big_integer const &a, big_integer const &b) {
    return b < a;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return !(a > b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return !(b > a);
}

std::string to_string(big_integer const &a) {
    std::string ans;

    big_integer b = a.abs();
    do {
        std::pair<big_integer, big_integer> p = divmod(b, 10);
        ans += std::to_string((p.second).digits[0]);
        b = p.first;
    } while (b > ZERO);

    if (a.sign) {
        ans += "-";
    }
    std::reverse(ans.begin(), ans.end());
    return ans;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}
