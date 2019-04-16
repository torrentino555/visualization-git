#ifndef _STATES_H_
#define _STATES_H_
#define GLM_ENABLE_EXPERIMENTAL

#include "ShaderProgram.h"
#include "PointAndLine.h"
#include "Tree.h"

class Tree;

class States {
private:
    float lastScale = 1.0f;
    std::chrono::steady_clock::time_point timeStartUpdateScale;
    Tree *tree;
    GLuint VBO, VAO;

    std::vector<Line> bufferLines;
    std::vector<Point> bufferCircles;
public:
    bool autoZoom = true;
    ShaderProgram *shaderProgram = nullptr;
    GLfloat scale = 1.0f;
    GLfloat offsetX = 0.0f, offsetY = 0.0f;

    States(std::string);

    void initGraphic();

    void render();
};

#endif