#pragma once

#include "SFML/Graphics.hpp"

#include <cmath>

template<typename T>
double CalcSqLength(sf::Vector2<T> vector) {
    return vector.x * vector.x + vector.y * vector.y;
}

template<typename T>
double CalcLength(sf::Vector2<T> vector) {
    return std::sqrt(CalcSqLength(vector));
}

template<typename T>
sf::Vector2<T> operator*(sf::Vector2<T> v, float k) {
    v.x *= k;
    v.y *= k;
    return v;
}
template<typename T>
sf::Vector2<float> Cast(sf::Vector2<T> v) {
    return {v.x, v.y};
}

// template<typename T>
// sf::Vector2<T> operator/(sf::Vector2<T> v, float k) {
//     v.x /= k;
//     v.y /= k;
//     return v;
// }


template<typename T1, typename T2>
sf::Vector2<double> operator+(sf::Vector2<T1> v, sf::Vector2<T2> v2) {
    sf::Vector2<double> result;
    result.x = v.x + v2.x;
    result.y = v.y + v2.y;
    return result;
}

template<typename T1, typename T2>
sf::Vector2<double> operator-(sf::Vector2<T1> v, sf::Vector2<T2> v2) {
    sf::Vector2<double> result;
    result.x = v.x - v2.x;
    result.y = v.y - v2.y;
    return result;
}

