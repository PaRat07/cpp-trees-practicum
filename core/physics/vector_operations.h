#pragma once

#include "SFML/Graphics.hpp"

#include <cmath>

float CalcSqLength(sf::Vector2f vector);

float CalcLength(sf::Vector2f vector);

sf::Vector2f operator*(sf::Vector2f v, float k);
