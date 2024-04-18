#pragma once

#include <iostream>

#include "SFML/Graphics.hpp"
#include <future>
#include <optional>
#include <cmath>

#include "center_positioned_string.h"
#include "../app/tab.h"
#include "../app/general_data.h"
#include "rounded_rectangle.h"
#include "inoutput_field.h"
#include "button.h"

class Table {
public:
    virtual sf::String GetValue(sf::Vector2i pos) const = 0;
    virtual sf::Vector2i Size() const = 0;
    virtual void SetValue(sf::Vector2i pos, sf::String value) = 0;
    virtual sf::String GetColumnName(int pos) const = 0;
    virtual sf::String GetLineName(int pos) const = 0;
    virtual void EraseColumn(int pos) = 0;
    virtual void EraseLine(int pos) = 0;
};

