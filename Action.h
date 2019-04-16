#ifndef _ACTION_H_
#define _ACTION_H_
#pragma once
#include <string>
#include "Tree.h"

class Leaf;
class Curve;

struct Action {
    size_t index = static_cast<size_t>(-1);
    std::string type;
    float startX, startY, endX, endY;
    float startSecondX, startSecondY, endSecondX, endSecondY;
    Leaf* l;
    Curve* curve;
    bool moveToUp = true;
};

#endif