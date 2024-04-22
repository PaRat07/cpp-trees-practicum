#include "../core/physics/vector_operations.h"

float CalcSqLength(sf::Vector2f vector) {
    return vector.x * vector.x + vector.y * vector.y;
}

float CalcLength(sf::Vector2f vector) {
    return std::sqrt(CalcSqLength(vector));
}

sf::Vector2f operator*(sf::Vector2f v, float k) {
    v.x *= k;
    v.y *= k;
    return v;
}
