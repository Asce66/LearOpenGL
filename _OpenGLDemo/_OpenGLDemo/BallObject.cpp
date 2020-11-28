#include "BallObject.h"

BallObject::BallObject():GameObject(),Radius(12.5f),Stuck(true)
{
}

BallObject::BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D& sprite) :
    GameObject(pos, glm::vec2(radius * 2, radius * 2), sprite, glm::vec3(1),
        velocity), Radius(12.5f), Stuck(true), Sticky(false), PassThrough(false)
{
}

glm::vec2 BallObject::Move(GLfloat dt, GLuint window_width)
{
    if (!Stuck)
    {
        Position +=  Velocity* dt;
        if (Position.x <= 0)
        {
            Velocity.x = -Velocity.x;
            Position.x = 0;
        }
        else if (Position.x + Size.x > window_width) 
        {
            Velocity.x = -Velocity.x;
            Position.x = window_width-Size.x;
        }
        if (Position.y <= 0)
        {
            Velocity.y = -Velocity.y;
            Position.y = 0;
        }
    }
    return Position;
}

void BallObject::Reset(glm::vec2 pos, glm::vec2 velocity)
{
    Position = pos;
    Velocity = velocity;
    Stuck = true; 
}
