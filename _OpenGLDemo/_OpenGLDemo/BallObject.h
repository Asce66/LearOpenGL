#pragma once
#include "GameObject.h"
#include"texture.h"
class BallObject :
    public GameObject
{
public:
    //球的状态
    GLfloat Radius;
    GLboolean Stuck;//球是否停止不动
    GLboolean PassThrough;//是否可以碰到砖块后不反弹继续向前冲
    GLboolean Sticky;//球是否获得stick道具，在下次碰到板后会被黏住

    BallObject();
    BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D& sprite);
    glm::vec2 Move(GLfloat dt, GLuint window_width);
    void Reset(glm::vec2 pos, glm::vec2 velocity);
};

