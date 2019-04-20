#include <utility>

#include <utility>

#include "Tree.h"
#include "States.h"
#include "Constants.h"
#include "Utils.h"
#include <iostream>
#include <set>
#include <stack>
#include <algorithm>
#include <glm/gtx/string_cast.hpp>
#include <git2.h>
#include "Utils.h"
#include "RGBA.h"

void Tree::checkMax(float x, float y) {
    xMin = min(xMin, x - CIRCLE_D);
    yMin = min(yMin, y - CIRCLE_D);
    xMax = max(xMax, x + CIRCLE_D);
    yMax = max(yMax, y + CIRCLE_D);
}

void Tree::drawCircle(Leaf *l, GLfloat x, GLfloat y, const std::string &colorString = "white") {
    // находим макс мин для масштабирования
    checkMax(x, y);

    l->index = bufferCircles->size();
    size_t i = 0;
    GLfloat step = 2 * M_PI / CIRCLE_COUNT_DIVISIONS;
    struct RGBA color = RGBA();
    if (colorString == "purple") {
        color = {204.0 / 255, 0.0f, 204.0 / 255, 1.0f};
    } else if (colorString == "white") {
        color = {1, 1, 1, 1};
    } else {
        std::cout << "Неопознанный цвет: " << colorString << std::endl;
    }

    for (float degree = 0; i < CIRCLE_COUNT_DIVISIONS; i++, degree += step) {
        bufferCircles->push_back(Point{x + cos(degree) * CIRCLE_RADIUS, y + sin(degree) * CIRCLE_RADIUS, color});
        bufferCircles->push_back(Point{x, y, color});
    }
    bufferCircles->push_back(Point{x + CIRCLE_RADIUS, y, color});
}

void Tree::drawLine(Curve *c, float x1, float y1, float x2, float y2, const std::string &colorString = "white") {
    struct RGBA color = RGBA();
    if (colorString == "white") {
        color = {1, 1, 1, 1};
    } else if (colorString == "purple") {
        color = {204.0 / 255, 0.0f, 204.0 / 255, 1};
    }
    c->index = bufferLines->size();
    bufferLines->push_back(Line{
            Point{x1, y1, color},
            Point{x2, y2, color}
    });
}

Tree::Tree(std::string full_path, std::vector<Point> *bufferCircles, std::vector<Line> *bufferLines)
        : bufferLines(bufferLines), bufferCircles(bufferCircles) {
    animations = new Animations(bufferLines, bufferCircles);
    git = new WorkWithGit(std::move(full_path), this);

    bufferLines->clear();
    bufferCircles->clear();

    if (git->next_commit()) {
        animations->clear();
        git->get_diffs();
        // addFile("kek");
        // addFile("a/a");
        // addFile("c/a");
        // addFile("d/a");
        // addFile("d/k/a");
        // addFile("c/k/a");

        lBorder = rBorder = 0.0f;
        preDraw(&root, 0, 0, "root");

        animations->setStartTime();
    } else {
        std::cout << "В текущей ветке проекта не найдено коммитов" << std::endl;
    }
}

void Tree::addFile(const std::string &path) {
    std::cout << "Add " << path << std::endl;
    addPath(&root, path);
}

void Tree::removeFile(const std::string &path) {
    std::cout << "Remove " << path << std::endl;
    deletePath(&root, path);
}

void Tree::modifyFile(const std::string &path) {
    std::cout << "Modify " << path << std::endl;
    modifyPath(&root, path);
}

void Tree::postDrawAndLine(float x, float y, float childX, float childY, Curve *curve, std::string direction) {
    drawLine(curve, x, y, childX, childY, "purple");
    postDraw(&curve->vertice, childX, childY, std::move(direction));
}

void Tree::drawLeafs(std::vector<Leaf *> *leafs, GLfloat x, GLfloat y) {
    size_t leafsLeft = leafs->size();
    size_t leafsIndex = 0;

    if (leafsLeft == 2) {
        drawCircle((*leafs)[0], x, y + CIRCLE_RADIUS, "purple");
        drawCircle((*leafs)[1], x, y - GAP_BETWEEN_CIRCLES - CIRCLE_RADIUS, "purple");
        return;
    }

    if (leafsLeft > 0) {
        leafsLeft--;
        drawCircle((*leafs)[leafsIndex++], x, y, "purple");
    }

    size_t level = 1;
    while (leafsLeft > 0) {
        level++;
        size_t countCirclesOnLev = countCirclesOnLevel(level);
        size_t countCircles = min(leafsLeft, countCirclesOnLev);
        leafsLeft -= countCircles;

        GLfloat degrees = M_PI / 2;
        if (countCircles == countCirclesOnLev && level % 2 == 0) {
            // на четных уровнях листья немного смещены относительно 90 градусов
            degrees += (CIRCLE_RADIUS + GAP_BETWEEN_CIRCLES / 2) / ((level - 1) * (CIRCLE_D + GAP_BETWEEN_CIRCLES));
        }

        for (size_t i = 0; i < countCircles; i++, degrees += 2 * M_PI / countCircles) {
            drawCircle((*leafs)[leafsIndex++],
                       x + cos(degrees) * ((level - 1) * (CIRCLE_D + GAP_BETWEEN_CIRCLES)),
                       y + sin(degrees) * ((level - 1) * (CIRCLE_D + GAP_BETWEEN_CIRCLES)), "purple");
        }
    }
}

void Tree::postDraw(Vertice *v, float x, float y, const std::string &direction) {
    for (auto curve : v->curves_to_add) {
        v->curvesMap[curve.second->label] = curve.second;
        v->curves.push_back(curve.second);
    }
    v->curves_to_add.clear();

    for (auto curve : v->curves_to_delete) {
        auto itDeleteItem = std::find_if(v->curves.begin(), v->curves.end(), [curve](auto curve_in) {
            return curve_in->label == curve;
        });
        v->curves.erase(itDeleteItem);
        v->curvesMap.erase(curve);
    }
    v->curves_to_delete.clear();

    for (auto leaf : v->leafs_to_add) {
        v->leafs.push_back(leaf);
    }
    v->leafs_to_add.clear();

    for (auto label : v->leafs_to_delete) {
        auto itDeleteItem = std::find_if(v->leafs.begin(), v->leafs.end(), [label](auto leaf) {
            return leaf->label == label;
        });
        v->leafs.erase(itDeleteItem);
    }
    v->leafs_to_delete.clear();

    v->new_leafs_count = v->leafs.size();
    v->oldX = x;
    v->oldY = y;

    drawLeafs(&v->leafs, x, y);

    float radius = calculateRadius(v->leafs.size());
    float curvesSize = v->curves.size();

    if (curvesSize > 0) {
        postDrawAndLine(x, y,
                        x,
                        y - radius - GAP_BETWEEN_BRANCHES * 10 - calculateRadius(v->curves[0]->vertice.new_leafs_count),
                        v->curves[0], direction);

        for (auto it = ++v->curves.begin(); it != v->curves.end(); it++) {
            Curve *curve = *it;

            float childRadius = calculateMaxColumnRadius(&curve->vertice);
            float childY = y - radius - GAP_BETWEEN_BRANCHES * 10 - childRadius;

            if (direction == "center") {
                if (std::distance(v->curves.begin(), it) % 2 == 1) {
                    postDrawAndLine(x, y, lBorder - GAP_BETWEEN_BRANCHES - childRadius, childY, curve, "left");
                } else {
                    postDrawAndLine(x, y, rBorder + GAP_BETWEEN_BRANCHES + childRadius, childY, curve, "right");
                }
            } else if (direction == "left") {
                postDrawAndLine(x, y, lBorder - GAP_BETWEEN_BRANCHES - childRadius, childY, curve, direction);
            } else if (direction == "right") {
                postDrawAndLine(x, y, rBorder + GAP_BETWEEN_BRANCHES + childRadius, childY, curve, direction);
            }
        }
    }

    float possibleLBorder = x - radius;
    float possibleRBorder = x + radius;

    if (possibleLBorder < lBorder) {
        lBorder = possibleLBorder;
    }

    if (possibleRBorder > rBorder) {
        rBorder = possibleRBorder;
    }
}

bool Tree::curveIsEmpty(Curve *curve, float x, float y, float childX, float childY) {
    while (curve->vertice.new_leafs_count == 0) {
        if (curve->vertice.curves.size() - curve->vertice.curves_to_delete.size() == 0 &&
            !curve->vertice.curves_to_add.empty()) {
            auto *action = new Action{curve->index, Animations::ADD_CURVE, x, y, childX, childY};
            action->curve = curve;
            animations->addAction(action);
            action->type = Animations::REMOVE_CURVE;
            animations->addAction(action);
        }

        size_t curveSize = curve->vertice.curves.size() + curve->vertice.curves_to_add.size() -
                           curve->vertice.curves_to_delete.size();
        if (curveSize == 0) {
            return true;
        } else if (curveSize == 1) {
            if (!curve->vertice.curves.empty()) {
                curve = *curve->vertice.curves.begin();
            } else if (!curve->vertice.curves_to_add.empty()) {
                curve = curve->vertice.curves_to_add.begin()->second;
            } else {
                std::cout << "ОШИБКА, НАДО ФИКСИТЬ" << std::endl;
            }
        } else {
            break;
        }
    }

    return false;
}

// void Tree::deleteStage(Vertice* v) {
//   bool isEmpty = true;
//   for (auto curve : v->curves) {
//     deleteStage(&curve->vertice);
//     if (!curve->vertice.isEmpty) {
//       isEmpty = false;
//     } else {
//       animations->addAction(new Action{ curve->index, Animations })
//       if (curve->vertice.curves_to_add.size() != 0 && curve->vertice.leafs_to_add.size() != 0) {
//         v->curves_to_delete.push_back(curve->label);
//       } else {

//       }
//     }
//   }

//   if (v->leafs.size() - v->leafs_to_delete.size() != 0) {
//     isEmpty = false;
//   }

//   v->isEmpty = isEmpty;
// }

void Tree::preDraw(Vertice *v, float x, float y, std::string direction) {
    std::vector<Curve *> newCurves(v->curves.begin(), v->curves.end());
    for (auto curve : v->curves_to_delete) {
        auto itDeleteItem = std::find_if(v->curves.begin(), v->curves.end(), [curve](auto curve_in) {
            return curve_in->label == curve;
        });
        newCurves.erase(itDeleteItem);
    }
    for (const auto &curve : v->curves_to_add) {
        newCurves.push_back(curve.second);
    }

    std::vector<Leaf *> leafs(v->leafs.begin(), v->leafs.end());
    for (const auto &label : v->leafs_to_delete) {
        auto itDeleteItem = std::find_if(leafs.begin(), leafs.end(), [label](auto leaf) {
            return leaf->label == label;
        });
        leafs.erase(itDeleteItem);
    }
    for (auto leaf : v->leafs_to_add) {
        leafs.push_back(leaf);
    }

    float radius = calculateRadius(leafs.size());
    if (direction == "root") {
        x = 0;
        y = 1 - GAP_BETWEEN_BRANCHES - radius;
        direction = "center";
    }


    if (!v->leafs_to_add.empty() || !v->leafs_to_delete.empty()) {
        // сдвинуть новые на нужные координаты
        // сдвинуть старые но новые места ( начиная сдвиг уже с новых координат )
        redrawLeafs(&leafs, x, y, x - v->oldX, y - v->oldY);
    }

    if (x != v->oldX || y != v->oldY) {
        // сдвиг листьев от начальной координаты к новой
        moveLeafs(&leafs, x - v->oldX, y - v->oldY);
    }

    size_t curvesSize = newCurves.size();
    if (curvesSize > 0) {
        auto startIt = newCurves.begin();
        float childY = y - radius - GAP_BETWEEN_BRANCHES * 10 - calculateRadius((*startIt)->vertice.new_leafs_count);
        while (startIt != newCurves.end()) {
            if (curveIsEmpty((*startIt), x, y, x, childY)) {
                v->curves_to_delete.push_back((*startIt)->label);
                animations->addAction(new Action{(*startIt)->index, Animations::REMOVE_CURVE});
                startIt++;
            } else {
                predrawVerticeAndLine(x, y, x, childY, (*startIt), direction);
                break;
            }
        }

        if (startIt != newCurves.end()) {
            size_t i = 1;
            for (auto it = ++startIt; it != newCurves.end(); it++, i++) {
                if (direction == "center") {
                    if (i % 2 == 1) {
                        while (it != newCurves.end()) {
                            Curve *curve = *it;
                            float childRadius = calculateMaxColumnRadius(&curve->vertice);
                            float childX = lBorder - GAP_BETWEEN_BRANCHES - childRadius;
                            childY = y - radius - GAP_BETWEEN_BRANCHES * 10 - childRadius;
                            if (curveIsEmpty(*it, x, y, childX, childY)) {
                                v->curves_to_delete.push_back((*it)->label);
                                animations->addAction(new Action{(*it)->index, Animations::REMOVE_CURVE});
                                it++;
                            } else {
                                predrawVerticeAndLine(x, y, childX, childY, (*it), "left");
                                break;
                            }
                        }
                    } else {
                        while (it != newCurves.end()) {
                            Curve *curve = *it;
                            float childRadius = calculateMaxColumnRadius(&curve->vertice);
                            float childX = rBorder + GAP_BETWEEN_BRANCHES + childRadius;
                            childY = y - radius - GAP_BETWEEN_BRANCHES * 10 - childRadius;
                            if (curveIsEmpty((*it), x, y, childX, childY)) {
                                v->curves_to_delete.push_back((*it)->label);
                                animations->addAction(new Action{(*it)->index, Animations::REMOVE_CURVE});
                                it++;
                            } else {
                                predrawVerticeAndLine(x, y, childX, childY, (*it), "right");
                                break;
                            }
                        }
                    }
                } else if (direction == "left") {
                    while (it != newCurves.end()) {
                        Curve *curve = *it;
                        float childRadius = calculateMaxColumnRadius(&curve->vertice);
                        float childX = lBorder - GAP_BETWEEN_BRANCHES - childRadius;
                        childY = y - radius - GAP_BETWEEN_BRANCHES * 10 - childRadius;
                        if (curveIsEmpty((*it), x, y, childX, childY)) {
                            v->curves_to_delete.push_back((*it)->label);
                            animations->addAction(new Action{(*it)->index, Animations::REMOVE_CURVE});
                            it++;
                        } else {
                            predrawVerticeAndLine(x, y, childX, childY, (*it), direction);
                            break;
                        }
                    }
                } else if (direction == "right") {
                    while (it != newCurves.end()) {
                        Curve *curve = *it;
                        float childRadius = calculateMaxColumnRadius(&curve->vertice);
                        float childX = rBorder + GAP_BETWEEN_BRANCHES + childRadius;
                        childY = y - radius - GAP_BETWEEN_BRANCHES * 10 - childRadius;
                        if (curveIsEmpty((*it), x, y, childX, childY)) {
                            v->curves_to_delete.push_back((*it)->label);
                            animations->addAction(new Action{(*it)->index, Animations::REMOVE_CURVE});
                            it++;
                        } else {
                            predrawVerticeAndLine(x, y, childX, childY, (*it), direction);
                            break;
                        }
                    }
                }

                if (it == newCurves.end()) {
                    break;
                }
            }
        }
    }

    float possibleLBorder = x - radius;
    float possibleRBorder = x + radius;

    if (possibleLBorder < lBorder) {
        lBorder = possibleLBorder;
    }

    if (possibleRBorder > rBorder) {
        rBorder = possibleRBorder;
    }
}

void Tree::redrawLeafs(std::vector<Leaf *> *leafs, GLfloat x, GLfloat y, GLfloat deltaX, GLfloat deltaY) {
    size_t leafsLeft = leafs->size();
    size_t leafsIndex = 0;

    if (leafsLeft == 2) {
        drawOrMoveCircle((*leafs)[0], x, y + CIRCLE_RADIUS, deltaX, deltaY);
        drawOrMoveCircle((*leafs)[1], x, y - GAP_BETWEEN_CIRCLES - CIRCLE_RADIUS, deltaX, deltaY);
        return;
    }

    if (leafsLeft > 0) {
        leafsLeft--;
        drawOrMoveCircle((*leafs)[leafsIndex++], x, y, deltaX, deltaY);
    }

    size_t level = 1;
    while (leafsLeft > 0) {
        level++;
        size_t countCirclesOnLev = countCirclesOnLevel(level);
        size_t countCircles = min(leafsLeft, countCirclesOnLev);
        leafsLeft -= countCircles;

        GLfloat degrees = M_PI / 2;
        if (countCircles == countCirclesOnLev && level % 2 == 0) {
            // на четных уровнях листья немного смещены относительно 90 градусов
            degrees += (CIRCLE_RADIUS + GAP_BETWEEN_CIRCLES / 2) / ((level - 1) * (CIRCLE_D + GAP_BETWEEN_CIRCLES));
        }

        for (size_t i = 0; i < countCircles; i++, degrees += 2 * M_PI / countCircles) {
            drawOrMoveCircle((*leafs)[leafsIndex++],
                             x + cos(degrees) * ((level - 1) * (CIRCLE_D + GAP_BETWEEN_CIRCLES)),
                             y + sin(degrees) * ((level - 1) * (CIRCLE_D + GAP_BETWEEN_CIRCLES)), deltaX, deltaY);
        }
    }
}

void Tree::drawOrMoveCircle(Leaf *leaf, GLfloat x, GLfloat y, GLfloat deltaX, GLfloat deltaY) {
    if (leaf->newLeaf) {
        leaf->newLeaf = false;
        checkMax(x, y);
        auto *action = new Action{(size_t) -1, Animations::ADD_LEAF, x, y};
        action->l = leaf;
        animations->addAction(action);
    } else {
        // подвинуть на новые корды с новым центром
        animations->addAction(new Action{leaf->index, Animations::TRANSLATE_LEAF,
                                         (*bufferCircles)[leaf->index + 1].x + deltaX,
                                         (*bufferCircles)[leaf->index + 1].y + deltaY, x, y});
    }
}

void Tree::moveLeafs(std::vector<Leaf *> *leafs, GLfloat deltaX, GLfloat deltaY) {
    for (auto leaf : *leafs) {
        if (leaf->index != (size_t) -1) {
            checkMax((*bufferCircles)[leaf->index + 1].x + deltaX, (*bufferCircles)[leaf->index + 1].y + deltaY);
            animations->addAction(new Action{leaf->index, Animations::MOVE_LEAF,
                                             (*bufferCircles)[leaf->index + 1].x, (*bufferCircles)[leaf->index + 1].y,
                                             (*bufferCircles)[leaf->index + 1].x + deltaX,
                                             (*bufferCircles)[leaf->index + 1].y + deltaY});
        }
    }
}

float Tree::calculateMaxColumnRadius(Vertice *v) {
    float maxColumnRadius = calculateRadius(v->new_leafs_count);
    while (v->curves.size() > 0) {
        v = &v->curves[0]->vertice;
        maxColumnRadius = max(maxColumnRadius, calculateRadius(v->new_leafs_count));
    }

    return maxColumnRadius;
}

float Tree::calculateRadius(size_t leafsCount) {
    if (leafsCount == 0) {
        return CIRCLE_RADIUS;
    }

    size_t level = 1;
    while (leafsCount > 0) {
        level++;
        size_t countCirclesOnLev = countCirclesOnLevel(level);
        size_t countCircles = min(leafsCount, countCirclesOnLev);
        leafsCount -= countCircles;
    }

    return ((level - 1) * (CIRCLE_D + GAP_BETWEEN_CIRCLES)) + CIRCLE_RADIUS;
}

size_t Tree::countCirclesOnLevel(const size_t level) {
    return level > 0 ? (level == 1 ? 1 : 3 * level) : 0;
}

void Tree::predrawVerticeAndLine(float x, float y, float childX, float childY, Curve *curve, std::string direction) {
    if (curve->index == (size_t) -1) {
        Action *action = new Action{curve->index, Animations::ADD_CURVE, x, y, childX, childY};
        action->curve = curve;
        animations->addAction(action);
    } else if ((*bufferLines)[curve->index].first.x != x ||
               (*bufferLines)[curve->index].first.y != y ||
               (*bufferLines)[curve->index].second.x != childX ||
               (*bufferLines)[curve->index].second.y != childY) {
        animations->addAction(
                new Action{curve->index, Animations::MOVE_CURVE,
                           (*bufferLines)[curve->index].first.x, (*bufferLines)[curve->index].first.y,
                           x, y,
                           (*bufferLines)[curve->index].second.x, (*bufferLines)[curve->index].second.y,
                           childX, childY});
    }
    preDraw(&curve->vertice, childX, childY, direction);
}

void Tree::addPath(Vertice *v, std::string path) {
    size_t index_r_limit = path.find("/");

    if (index_r_limit != (size_t) -1) {
        std::string curveLabel = path.substr(0, index_r_limit);
        index_r_limit++;
        if (v->curvesMap.find(curveLabel) != v->curvesMap.end()) {
            addPath(&v->curvesMap.at(curveLabel)->vertice, path.substr(index_r_limit));
        } else if (v->curves_to_add.find(curveLabel) != v->curves_to_add.end()) {
            addPath(&v->curves_to_add.at(curveLabel)->vertice, path.substr(index_r_limit));
        } else {
            Curve *curve = new Curve(curveLabel);
            v->curves_to_add[curveLabel] = curve;
            addPath(&(curve->vertice), path.substr(index_r_limit));
        }

        return;
    }
    v->new_leafs_count++;
    v->leafs_to_add.push_back(new Leaf(path));
}

void Tree::deletePath(Vertice *v, std::string path) {
    size_t index_r_limit = path.find("/");

    if (index_r_limit != (size_t) -1) {
        std::string curveLabel = path.substr(0, index_r_limit);
        Curve *curve = v->curvesMap.at(curveLabel);
        index_r_limit++;
        deletePath(&curve->vertice, path.substr(index_r_limit));

        return;
    }

    v->new_leafs_count--;

    auto itDeleteItem = std::find_if(v->leafs.begin(), v->leafs.end(), [path](auto leaf) {
        return leaf->label == path;
    });
    animations->addAction(new Action{(*itDeleteItem)->index, Animations::REMOVE_LEAF});

    v->leafs_to_delete.push_back(path);
}

void Tree::modifyPath(Vertice *v, std::string path) {
    size_t index_r_limit = path.find("/");

    if (index_r_limit != (size_t) -1) {
        std::string curveLabel = path.substr(0, index_r_limit);
        index_r_limit++;
        Curve *curve = v->curvesMap.at(curveLabel);
        modifyPath(&curve->vertice, path.substr(index_r_limit));
        return;
    }

    auto itModifyItem = std::find_if(v->leafs.begin(), v->leafs.end(), [path](auto leaf) {
        return leaf->label == path;
    });

    animations->addAction(new Action{(*itModifyItem)->index, Animations::MODIFY_LEAF});
}

void Tree::render() {
    if (animations->getDeltaTime() > Animations::timeLimitAdd) {
        bufferCircles->clear();
        bufferLines->clear();
        lBorder = rBorder = 0.0f;
        xMin = yMin = -1.0f;
        xMax = yMax = 1.0f;
        postDraw(&root, 0, 1 - GAP_BETWEEN_BRANCHES - calculateRadius(root.new_leafs_count), "center");

        if (git->next_commit()) {
            std::cout << "-----------------\n";
            animations->clear();
            git->get_diffs();
            // if (k) {
            //   removeFile("a/a");
            //   k = false;
            // }

            lBorder = rBorder = 0.0f;
            preDraw(&root, 0, 0, "root");

            animations->setStartTime();
        }
    } else {
        animations->render();
    }
}