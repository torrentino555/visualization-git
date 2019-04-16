#include "Animations.h"
#include "Constants.h"

const std::string Animations::REMOVE_LEAF = "remove-leaf";
const std::string Animations::ADD_LEAF = "add-leaf";
const std::string Animations::REMOVE_CURVE = "remove-curve";
const std::string Animations::MOVE_LEAF = "move-leaf";
const std::string Animations::MOVE_CURVE = "move-curve";
const std::string Animations::ADD_CURVE = "add-curve";
const std::string Animations::MODIFY_LEAF = "modify-leaf";
const std::string Animations::TRANSLATE_LEAF = "translate-leaf";
size_t Animations::timeLimitDelete = 500;
size_t Animations::timeLimitMove = 1000;
size_t Animations::timeLimitAdd = 1500;

size_t Animations::durationDelete = 500;
size_t Animations::durationMove = 500;
size_t Animations::durationAdd = 500;

void Animations::modifyLeafAnimation(Action* action, size_t milisecTime) {
    float koef = (milisecTime - timeLimitMove)/(float)durationDelete;
    for (size_t i = action->index; i < action->index + CIRCLE_COUNT_DIVISIONS*2 + 1; i++) {
        Point *currentP = &(*bufferCircles)[i];
        if (milisecTime == durationDelete) {
            currentP->x =  currentP->y = 3.0f;
            currentP->color.r = currentP->color.g = currentP->color.b = 1.0f;
        } else {
            currentP->color.r = 1.0f - (1.0f - modifyColor.r)*koef;
            currentP->color.g = 1.0f - (1.0f - modifyColor.g)*koef;
            currentP->color.b = 1.0f - (1.0f - modifyColor.b)*koef;
        }
    }
}

void Animations::removeLeafAnimation(Action* action, size_t milisecTime) {
    float koef = 1 - milisecTime/(float)durationDelete;
    for (size_t i = action->index; i < action->index + CIRCLE_COUNT_DIVISIONS*2 + 1; i++) {
        Point *currentP = &(*bufferCircles)[i];
        if (milisecTime == durationDelete) {
            currentP->x =  currentP->y = 3.0f;
            currentP->color.r = currentP->color.g = currentP->color.b = 1.0f;
        } else {
            currentP->color.r = 1.0f - (1.0f - removeColor.r)*koef;
            currentP->color.g = 1.0f - (1.0f - removeColor.g)*koef;
            currentP->color.b = 1.0f - (1.0f - removeColor.b)*koef;
        }
    }
}

void Animations::addLeafAnimation(Action* action, size_t milisecTime) {
    if (action->index == (size_t)-1) {
        action->l->index = action->index = drawCircle(action->startX, action->startY);
    }
    float koef = (milisecTime - timeLimitMove)/(float)durationDelete;
    for (size_t i = action->index; i < action->index + CIRCLE_COUNT_DIVISIONS*2 + 1; i++) {
        Point *currentP = &(*bufferCircles)[i];
        currentP->color.r = 1.0f - (1.0f - addColor.r)*koef;
        currentP->color.g = 1.0f - (1.0f - addColor.g)*koef;
        currentP->color.b = 1.0f - (1.0f - addColor.b)*koef;
    }
}

void Animations::addCurveAnimation(Action* action, size_t milisecTime) {
    if (action->index == (size_t)-1) {
        action->curve->index = action->index = drawLine(action->startX, action->startY, action->endX, action->endY);
    }
    float koef = (milisecTime - timeLimitMove)/(float)durationDelete;
    Point *first = &(*bufferLines)[action->index].first;
    Point *second = &(*bufferLines)[action->index].second;
    first->color.r = second->color.r = 1.0f - (1.0f - addColor.r)*koef;
    first->color.g = second->color.g = 1.0f - (1.0f - addColor.g)*koef;
    first->color.b = second->color.b = 1.0f - (1.0f - addColor.b)*koef;
}

void Animations::removeCurveAnimation(Action* action, size_t milisecTime) {
    if (action->index == (size_t)-1) {
        action->curve->index = action->index = drawLine(action->startX, action->startY, action->endX, action->endY);
    }

    float koef = 1 - milisecTime/(float)durationDelete;
    Point *first = &(*bufferLines)[action->index].first;
    Point *second = &(*bufferLines)[action->index].second;

    if (milisecTime == durationDelete) {
        first->x = second->x = first->y = second->y = 3.0f;
        first->color.r = second->color.r = first->color.g = second->color.g = first->color.b = second->color.b = 1.0f;
        return;
    }

    first->color.r = second->color.r = 1.0f - (1.0f - removeColor.r)*koef;
    first->color.g = second->color.g = 1.0f - (1.0f - removeColor.g)*koef;
    first->color.b = second->color.b = 1.0f - (1.0f - removeColor.b)*koef;
}

void Animations::moveLeafAnimation(Action* action, size_t milisecTime, bool last = false) {
    if (last) {
        for (size_t i = action->index; i < action->index + CIRCLE_COUNT_DIVISIONS*2 + 1; i++) {
            Point *currentP = &(*bufferCircles)[i];
            currentP->x += action->endX;
            currentP->y += action->endY;
        }
        return;
    }

    size_t duration = action->type == MOVE_LEAF ? durationMove : durationAdd;
    float deltaX = (action->endX - action->startX)*(milisecTime/(float)duration);
    float deltaY = (action->endY - action->startY)*(milisecTime/(float)duration);
    for (size_t i = action->index; i < action->index + CIRCLE_COUNT_DIVISIONS*2 + 1; i++) {
        Point *currentP = &(*bufferCircles)[i];
        currentP->x += deltaX;
        currentP->y += deltaY;
    }
}

void Animations::moveCurveAnimation(Action* action, size_t milisecTime, bool last = false) {
    float firstDeltaX = (action->endX - action->startX)*(milisecTime/(float)durationMove);
    float firstDeltaY = (action->endY - action->startY)*(milisecTime/(float)durationMove);
    float secondDeltaX = (action->endSecondX - action->startSecondX)*(milisecTime/(float)durationMove);
    float secondDeltaY = (action->endSecondY - action->startSecondY)*(milisecTime/(float)durationMove);

    Line* line = &(*bufferLines)[action->index];
    if (last) {
        line->first.x = action->endX;
        line->first.y += action->endY;
        line->second.x += action->endSecondX;
        line->second.y += action->endSecondY;
        return;    
    }

    line->first.x += firstDeltaX;
    line->first.y += firstDeltaY;
    line->second.x += secondDeltaX;
    line->second.y += secondDeltaY;
}

void Animations::render() {
    size_t timeInMiliseconds = getDeltaTime();
    size_t deltaTimeWithLastDraw = getDeltaTimeWithLastDraw();
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
        if (stage == "delete") {
            for (auto action : actions) {
                if (action.type == REMOVE_LEAF) {
                    removeLeafAnimation(&action, timeLimitDelete);
                }

                if (action.type == REMOVE_CURVE) {
                    removeCurveAnimation(&action, timeLimitDelete);
                }
            }
            stage = "move";
        }
        // вторая стадия, сдвиг
        for (auto action : actions) {
            if (action.type == MOVE_LEAF) {
                moveLeafAnimation(&action, deltaTimeWithLastDraw);
            }

            if (action.type == MOVE_CURVE) {
                moveCurveAnimation(&action, deltaTimeWithLastDraw);
            }
        }
    } else if (timeInMiliseconds <= timeLimitAdd) {
        if (stage == "move") {
            for (auto action : actions) {
                if (action.type == MOVE_LEAF) {
                    moveLeafAnimation(&action, deltaTimeWithLastDraw, true);
                }

                if (action.type == MOVE_CURVE) {
                    moveCurveAnimation(&action, deltaTimeWithLastDraw, true);
                }
            }
            stage = "add";
        }
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

            if (action.type == TRANSLATE_LEAF) {
                moveLeafAnimation(&action, deltaTimeWithLastDraw);
            }
        }
    } else if (stage == "add") {
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

            if (action.type == TRANSLATE_LEAF) {
                moveLeafAnimation(&action, deltaTimeWithLastDraw, true);
            }
        }
        stage = "delete";
    }
}

size_t Animations::drawCircle(GLfloat x, GLfloat y) {
  size_t i = 0, index = bufferCircles->size();
  auto step = static_cast<GLfloat>(2 * M_PI / CIRCLE_COUNT_DIVISIONS);
  struct RGBA color = { 1, 1, 1, 1 };

  for (float degree = 0; i < CIRCLE_COUNT_DIVISIONS; i++, degree += step) {
    bufferCircles->push_back(Point{ x + cos(degree) * CIRCLE_RADIUS, y + sin(degree) * CIRCLE_RADIUS, color.r, color.g, color.b, color.a});
    bufferCircles->push_back(Point{x, y, color.r, color.g, color.b, color.a});
  }
  bufferCircles->push_back(Point{x + CIRCLE_RADIUS, y, color.r, color.g, color.b, color.a});
  return index;
}

size_t Animations::drawLine(float x1, float y1, float x2, float y2) {
  struct RGBA color = { 1, 1, 1, 1 };
  bufferLines->push_back(Line{
    Point{x1, y1, color.r, color.g, color.b, color.a },
    Point{x2, y2, color.r, color.g, color.b, color.a }
  });
  return bufferLines->size() - 1;
}