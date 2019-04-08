#pragma once
#include <vector>
#include <chrono>
#include "Action.h"
#include "States.h"
#include "Utils.h"

struct RGBA {
    GLfloat r,g,b,a;
};

class Animations {
private:
    struct RGBA removeColor{ 220/255.0f, 20/255.0f, 60/255.0f, 1.0f };
    struct RGBA addColor{ 153/255.0f, 1.0f, 51/255.0f, 1.0f };
    struct RGBA modifyColor{ 1.0f, 1.0f, 51/255.0f, 1.0f };

    // в милисекундах
    size_t timeLimitDelete = 1000;
    size_t timeLimitMove = 1500;
    size_t timeLimitAdd = 2500;

    size_t durationDelete = 1000;
    size_t durationMove = 500;
    size_t durationAdd = 1000;

    std::vector<Action> actions;
    std::vector<Line>* bufferLines;
    std::vector<Point>* bufferCircles;
    std::chrono::steady_clock::time_point startTime;

    void modifyLeafAnimation(Action* action, size_t milisecTime);
    void addLeafAnimation(Action* action, size_t milisecTIme);
    void addCurveAnimation(Action* action, size_t milisecTIme);
    void removeLeafAnimation(Action* action, size_t milisecTIme);
    void removeCurveAnimation(Action* action, size_t milisecTIme);
    void moveLeafAnimation(Action* action, size_t milisecTIme);
    void moveCurveAnimation(Action* action, size_t milisecTIme);
public:
    const static std::string REMOVE_LEAF;
    const static std::string ADD_LEAF;
    const static std::string REMOVE_CURVE;
    const static std::string ADD_CURVE;
    const static std::string MOVE_LEAF;
    const static std::string MOVE_CURVE;
    const static std::string MODIFY_LEAF;

    Animations(std::vector<Line>* bufferLines, std::vector<Point>* bufferCircles) : bufferLines(bufferLines), bufferCircles(bufferCircles) {};

    void clear() { actions.clear(); };
    void render();
    void addAction(Action* action) { actions.push_back(*action); };
    void setStartTime() { startTime = std::chrono::steady_clock::now(); };
};

const std::string Animations::REMOVE_LEAF = "remove-leaf";
const std::string Animations::ADD_LEAF = "add-leaf";
const std::string Animations::REMOVE_CURVE = "remove-curve";
const std::string Animations::MOVE_LEAF = "move-leaf";
const std::string Animations::MOVE_CURVE = "move-curve";
const std::string Animations::ADD_CURVE = "add-curve";
const std::string Animations::MODIFY_LEAF = "modify-leaf";