#pragma once

#include "SFML/Graphics.hpp"
#include "../app/tab.h"

class HeaderDrawer : public AbstractDrawer {
public:
    virtual float Size() const = 0;
    virtual void SetShift(float shift) = 0;
    virtual ~HeaderDrawer() = default;
};