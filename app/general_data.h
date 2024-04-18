#pragma once

#include "SFML/Graphics.hpp"

sf::Font LoadFont(std::string path);


extern sf::Font font;
extern float letter_size;
extern sf::Vector2f win_size;
extern sf::Uint32 lambda;
extern long animation_time;
