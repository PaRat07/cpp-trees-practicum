#pragma once

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <future>
#include <optional>
#include <cmath>

#include "tab.h"
#include "general_data.h"
#include "colors.h"



class WindowManager {
public:
    WindowManager(Tab &&tab)
        : tab_(std::move(tab))
    {}

    void Start();

private:
    Tab tab_;
};
