#include "../core/monomial.h"

Monomial::Monomial()
        : factor(0)
{
    powers.fill(0);
}

Monomial::Monomial(const double &f, const std::array<int, 26> &p)
        : factor(f)
        , powers(p)
{}

std::strong_ordering Monomial::operator<=>(const Monomial &rhs) const {
    return std::lexicographical_compare_three_way(powers.begin(), powers.end(), rhs.powers.begin(), rhs.powers.end());
}

Monomial Monomial::operator-() const {
    return { -factor, powers };
}

Monomial Monomial::operator*(Monomial rhs) const {
    rhs.factor *= factor;
    std::transform(rhs.powers.begin(), rhs.powers.end(), powers.begin(), rhs.powers.begin(), std::plus<>{});
    return rhs;
}

bool Monomial::operator==(const Monomial &rhs) const {
    return std::equal(powers.begin(), powers.end(), rhs.powers.begin());
}

bool Monomial::operator!=(const Monomial &rhs) const {
    return !(*this == rhs);
}

std::string Monomial::ToString() const {
    if (factor == 0) {
        return "+0";
    }
    std::string ans;
    if (factor >= 0) {
        ans.push_back('+');
    }
    if (factor != 1 || std::accumulate(powers.begin(), powers.end(), 0) == 0) {
        std::string str = std::to_string(factor);
        while (!str.empty() && (str.back() == '.' || str.back() == '0')) {
            str.pop_back();
        }
        ans += str;
    }
    for (int i = 0; i < 26; ++i) {
        if (powers[i] > 0) {
            ans.push_back('a' + i);
            if (powers[i] > 1) {
                ans.push_back('^');
//                ans.push_back('{');
                ans += std::to_string(powers[i]);
//                ans.push_back('}');
            }
        }
    }
    return ans;
}

double Monomial::CalcValAt(const std::array<int, 26> &arr) const {
    return factor * std::transform_reduce(powers.begin(), powers.end(),
                                          arr.begin(),
                                          1, std::multiplies<>{},
                                          [] (int power, int val) {
                                              return std::pow(val, power);
                                          });
}

bool Mergeable(const Monomial &a, const Monomial &b) {
    return a == b;
}
