#include "game_object.h"

#ifndef BALL_OBJECT_H
#define BALL_OBJECT_H

class BallObject : public GameObject
{
public:
    // ÇòµÄ×´Ì¬ 
    GLfloat   Radius;
    GLboolean Stuck;
    GLboolean Sticky, PassThrough;
    
    BallObject();
    BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite);

    glm::vec2 Move(GLfloat dt, GLuint window_width);
    void      Reset(glm::vec2 position, glm::vec2 velocity);
};



#endif // !BALL_OBJECT_H
