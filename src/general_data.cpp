#include <iostream>
#include "../app/general_data.h"

sf::Font LoadFont(std::string path) {
    sf::Font ans;
    ans.loadFromFile(path);
    return ans;
}

sf::Font font = LoadFont("../../app/Roboto-Regular.ttf");
float letter_size = 20.f;