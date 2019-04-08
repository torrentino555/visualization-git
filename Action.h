#pragma once
#include <string>

struct Action {
    size_t index;
    std::string type;
    float startX, startY, endX, endY;
    float startSecondX, startSecondY;
};