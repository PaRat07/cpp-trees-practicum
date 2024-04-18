#include <iostream>
#include "../core/polynomial.h"

Polynomial::Polynomial(const std::string &s) {
    Parser parser(s);
    while (!parser.Empty()) {
        data_.push_back(parser.GetMonomial());
    }
    Normalize();
}

std::string Polynomial::ToString() const {
    std::string ans;
    for (const Monomial &m : data_) {
        ans = m.ToString() + ans;
    }
    return ans.empty() ? "0" : ans[0] != '+' ? ans : ans.substr(1);
}

Polynomial Polynomial::operator+(const Polynomial &rhs) const {
    Polynomial ans;
    auto it1 = data_.begin(), it2 = rhs.data_.begin();
    while (it1 != data_.end() && it2 != rhs.data_.end()) {
        if (*it1 < *it2) {
            ans.data_.push_back(*it1);
            ++it1;
        } else if (*it1 > *it2) {
            ans.data_.push_back(*it2);
            ++it2;
        } else {
            ans.data_.EmplaceBack(it1->factor + it2->factor, it1->powers);
            ++it1;
            ++it2;
        }
    }
    while (it1 != data_.end()) {
        ans.data_.push_back(*it1);
        ++it1;
    }
    while (it2 != rhs.data_.end()) {
        ans.data_.push_back(*it2);
        ++it2;
    }
    ans.Normalize();
    return ans;
}

Polynomial Polynomial::operator-(const Polynomial &rhs) const {
    Polynomial ans;
    std::for_each(rhs.data_.begin(), rhs.data_.end(), [&ans] (const Monomial &m) {
        ans.data_.push_back(-m);
    });
    std::for_each(data_.begin(), data_.end(), [&ans] (const Monomial &m) {
        ans.data_.push_back(m);
    });
    ans.Normalize();
    return ans;
}

Polynomial Polynomial::operator*(const Polynomial &rhs) const {
    Polynomial ans;
    std::for_each(data_.begin(), data_.end(), [&rhs, &ans] (const Monomial &m1) {
        std::for_each(rhs.data_.begin(), rhs.data_.end(), [&ans, &m1] (const Monomial &m2) {
            ans.data_.push_back(m1 * m2);
        });
    });
    ans.Normalize();
    return ans;
}

Polynomial &Polynomial::operator+=(const Polynomial &rhs) {
    return *this = *this + rhs;
}

Polynomial &Polynomial::operator-=(const Polynomial &rhs) {
    return *this = *this - rhs;
}

Polynomial &Polynomial::operator*=(const Polynomial &rhs) {
    return *this = *this * rhs;
}

Polynomial::DivisionResult Polynomial::operator/(const Polynomial &rhs) const {
    Polynomial this_copy = *this;
    DivisionResult ans;
    int letter = -1;
    for (const Monomial &m : data_) {
        for (int i = 0; i < 26; ++i) {
            if (m.powers[i] > 0) {
                if (letter == -1) {
                    letter = i;
                }
                if (letter != i) {
                    throw std::runtime_error("Called operator/ for some variables polynom");
                }
            }
        }
    }
    for (const Monomial &m : rhs.data_) {
        for (int i = 0; i < 26; ++i) {
            if (m.powers[i] > 0) {
                if (letter != i) {
                    throw std::runtime_error("Called operator/ for some variables polynom");
                }
            }
        }
    }
    DivisionResult res;
    while (!this_copy.data_.Empty() && this_copy.data_.Back().powers[letter] >= rhs.data_.Back().powers[letter]) {
        Monomial to_mul;
        to_mul.powers.fill(0);
        to_mul.powers[letter] = this_copy.data_.Back().powers[letter] - rhs.data_.Back().powers[letter];
        to_mul.factor = this_copy.data_.Back().factor / rhs.data_.Back().factor;
        res.whole_num.data_.PushBack(to_mul);
        Polynomial p;
        p.data_.PushBack(to_mul);
        this_copy -= rhs * p;
        this_copy.Normalize();
    }
    res.num = this_copy;
    res.den = rhs;
    res.whole_num.Normalize();
    return res;
}

double Polynomial::GetValueAt(const std::array<int, 26> &vals) const {
    return std::transform_reduce(data_.begin(), data_.end(), 0, std::plus<>{}, [&vals] (const Monomial &m) {
        return m.CalcValAt(vals);
    });
}

LinkedList<int> Polynomial::GetIntegerRoots() const {
    int letter = -1;
    for (const Monomial &m : data_) {
        for (int i = 0; i < 26; ++i) {
            if (m.powers[i] > 0) {
                if (letter == -1) {
                    letter = i;
                }
                if (letter != i) {
                    throw std::runtime_error("Called GetIntegerRoots for some variables polynom");
                }
            }
        }
    }
    LinkedList<int> ans;
    if (data_.Empty() || data_.Front().powers[letter] != 0) {
        return ans;
    }
    std::array<int, 26> vals;
    vals.fill(0);
    int factor_copy = data_.Front().factor;
    for (int i = 1; i * i <= factor_copy; ++i) {
        if (factor_copy % i == 0) {
            factor_copy /= i;
            vals[letter] = i;
            if (std::abs(GetValueAt(vals)) < 1e-6) {
                ans.push_back(i);
            }
            vals[letter] = -i;
            if (std::abs(GetValueAt(vals)) < 1e-6) {
                ans.push_back(-i);
            }
        }
    }
    vals[letter] = factor_copy;
    if (std::abs(GetValueAt(vals)) < 1e-6) {
        ans.push_back(factor_copy);
    }
    vals[letter] = -factor_copy;
    if (std::abs(GetValueAt(vals)) < 1e-6) {
        ans.push_back(-factor_copy);
    }
    return ans;
}

void Polynomial::Normalize() {
    data_.Sort();
    LinkedList<Monomial> new_data;
    for (const Monomial &m : data_) {
        if (m.factor == 0) continue;

        if (!new_data.Empty() && Mergeable(new_data.Back(), m)) {
            new_data.Back().factor += m.factor;
            if (new_data.Back().factor == 0) {
                new_data.Erase(--new_data.end());
            }
        } else {
            new_data.push_back(m);
        }
    }
    data_ = std::move(new_data);
}

Polynomial Polynomial::GetDerivative(char var) const {
    std::cerr << ToString() << std::endl;
    LinkedList<Monomial> ans;
    std::transform(data_.begin(), data_.end(), std::back_inserter(ans), [var] (const Monomial &m) {
        return m.GetDerivative(var - 'a');
    });
    Polynomial ans_p;
    ans_p.data_ = ans;
    return ans_p;
}

Polynomial operator ""_p(const char *data, size_t size) {
    return Polynomial(std::string(data, size));
}

std::ostream &operator<<(std::ostream &out, const Polynomial &p) {
    out << p.ToString();
    return out;
}

std::ostream &operator<<(std::ostream &out, const Polynomial::DivisionResult &dr) {
    out << dr.whole_num << " + (" << dr.num << ") / (" << dr.den << ")";
    return out;
}
