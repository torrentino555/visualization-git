#include "Animations.h"
#include "Constants.h"

void Animations::modifyLeafAnimation(Action* action, size_t milisecTime) {
    for (size_t i = action->index; i < action->index + CIRCLE_COUNT_DIVISIONS*2 + 1; i++) {
        Point *currentP = &(*bufferCircles)[i];
        currentP->r = modifyColor.r;
        currentP->g = modifyColor.g;
        currentP->b = modifyColor.b;
        currentP->a = (milisecTime - timeLimitMove)/durationDelete;
    }
}

void Animations::removeLeafAnimation(Action* action, size_t milisecTime) {
    for (size_t i = action->index; i < action->index + CIRCLE_COUNT_DIVISIONS*2 + 1; i++) {
        Point *currentP = &(*bufferCircles)[i];
        currentP->r = removeColor.r;
        currentP->g = removeColor.g;
        currentP->b = removeColor.b;
        currentP->a = 1 - milisecTime/durationDelete;
    }
}

void Animations::addLeafAnimation(Action* action, size_t milisecTime) {
    for (size_t i = action->index; i < action->index + CIRCLE_COUNT_DIVISIONS*2 + 1; i++) {
        Point *currentP = &(*bufferCircles)[i];
        currentP->r = addColor.r;
        currentP->g = addColor.g;
        currentP->b = addColor.b;
        currentP->a = (milisecTime - timeLimitMove)/durationDelete;
    }
}

void Animations::addCurveAnimation(Action* action, size_t milisecTime) {
    Point *first = &(*bufferLines)[action->index].first;
    Point *second = &(*bufferLines)[action->index].second;
    first->r = second->r = addColor.r;
    first->g = second->g = addColor.g;
    first->b = second->b = addColor.b;
    first->a = second->a = (milisecTime - timeLimitMove)/durationAdd;
}

void Animations::removeCurveAnimation(Action* action, size_t milisecTime) {
    Point *first = &(*bufferLines)[action->index].first;
    Point *second = &(*bufferLines)[action->index].second;
    first->r = second->r = removeColor.r;
    first->g = second->g = removeColor.g;
    first->b = second->b = removeColor.b;
    first->a = second->a = 1 - milisecTime/durationDelete;
}

void Animations::moveLeafAnimation(Action* action, size_t milisecTIme) {
    float deltaX = (action->endX - action->startX)*((milisecTIme - timeLimitDelete)/durationMove);
    float deltaY = (action->endY - action->startY)*((milisecTIme - timeLimitDelete)/durationMove);

    for (size_t i = action->index; i < action->index + CIRCLE_COUNT_DIVISIONS*2 + 1; i++) {
        Point *currentP = &(*bufferCircles)[i];
        currentP->x = action->startX + deltaX;
        currentP->y = action->startY + deltaY;
    }
}

void Animations::moveCurveAnimation(Action* action, size_t milisecTIme) {
    float deltaX = (action->endX - action->startX)*((milisecTIme - timeLimitDelete)/durationMove);
    float deltaY = (action->endY - action->startY)*((milisecTIme - timeLimitDelete)/durationMove);

    Line* line = &(*bufferLines)[action->index];
    line->first.x = action->startX + deltaX;
    line->first.y = action->startY + deltaY;
    line->second.x = action->startSecondX + deltaX;
    line->second.y = action->startSecondY + deltaY;
}

void Animations::render() {
    auto now = std::chrono::steady_clock::now();
    size_t timeInMiliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();
    if (timeInMiliseconds <= timeLimitDelete) {
        // перва стадия, удаление
        for (auto action : actions) {
            if (action.type == REMOVE_LEAF) {
                removeLeafAnimation(&action, timeInMiliseconds);
            }

            if (action.type == REMOVE_CURVE) {
                removeCurveAnimation(&action, timeInMiliseconds);
            }
        }
    } else if (timeInMiliseconds <= timeLimitMove) {
        // вторая стадия, сдвиг
        for (auto action : actions) {
            if (action.type == MOVE_LEAF) {
                moveLeafAnimation(&action, timeInMiliseconds);
            }

            if (action.type == MOVE_CURVE) {
                moveCurveAnimation(&action, timeInMiliseconds);
            }
        }
    } else if (timeInMiliseconds <= timeLimitAdd) {
        // третья стадия, добавление
        for (auto action : actions) {
            if (action.type == ADD_LEAF) {
                addLeafAnimation(&action, timeInMiliseconds);
            }

            if (action.type == ADD_CURVE) {
                addCurveAnimation(&action, timeInMiliseconds);
            }

            if (action.type == MODIFY_LEAF) {
                modifyLeafAnimation(&action, timeInMiliseconds);
            }
        }
    }
}
