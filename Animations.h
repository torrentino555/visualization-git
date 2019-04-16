#ifndef _ANIMATIONS_H_
#define _ANIMATIONS_H_
#include <vector>
#include <chrono>
#include "Action.h"
#include "States.h"
#include "Utils.h"
#include "Tree.h"
#include "PointAndLine.h"
#include "RGBA.h"

class Animations {
private:
    std::string stage = "delete";

    struct RGBA removeColor{ 220/255.0f, 20/255.0f, 60/255.0f, 1.0f };
    struct RGBA addColor{ 153/255.0f, 1.0f, 51/255.0f, 1.0f };
    struct RGBA modifyColor{ 1.0f, 1.0f, 51/255.0f, 1.0f };

    std::vector<Action> actions;
    std::vector<Line>* bufferLines;
    std::vector<Point>* bufferCircles;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastTime;

    void modifyLeafAnimation(Action* action, size_t milisecTime);
    void addLeafAnimation(Action* action, size_t milisecTIme);
    void addCurveAnimation(Action* action, size_t milisecTIme);
    void removeLeafAnimation(Action* action, size_t milisecTIme);
    void removeCurveAnimation(Action* action, size_t milisecTIme);
    void moveLeafAnimation(Action* action, size_t milisecTIme, bool);
    void moveCurveAnimation(Action* action, size_t milisecTIme, bool);
    size_t drawCircle(GLfloat x, GLfloat y);
    size_t drawLine(float x1, float y1, float x2, float y2);
public:
    const static std::string REMOVE_LEAF;
    const static std::string ADD_LEAF;
    const static std::string REMOVE_CURVE;
    const static std::string ADD_CURVE;
    const static std::string MOVE_LEAF;
    const static std::string TRANSLATE_LEAF;
    const static std::string MOVE_CURVE;
    const static std::string MODIFY_LEAF;
    // в милисекундах
    static size_t timeLimitDelete;
    static size_t timeLimitMove;
    static size_t timeLimitAdd;

    static size_t durationDelete;
    static size_t durationMove;
    static size_t durationAdd;

    Animations(std::vector<Line>* bufferLines, std::vector<Point>* bufferCircles) : bufferLines(bufferLines), bufferCircles(bufferCircles) {};

    void clear() { actions.clear(); };
    void render();
    void addAction(Action* action) { actions.push_back(*action); };
    void setStartTime() { startTime = std::chrono::steady_clock::now(); };
    size_t getDeltaTime() { return static_cast<size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - startTime).count()); }
    size_t getDeltaTimeWithLastDraw() {
        auto now = std::chrono::steady_clock::now();
        size_t result = static_cast<size_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                now - lastTime).count());
        lastTime = now;
        return result;
    }
};

#endif