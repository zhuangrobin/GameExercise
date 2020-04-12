#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite.h"

class GameObject
{
public:
    // Object state
    glm::vec2   Position, Size, Velocity;
    glm::vec3   Color;
    GLfloat     Rotation;
    GLboolean   IsSolid;
    GLboolean   Destroyed;
    // Render state
    Texture2D   Sprite;	
    // Constructor(s)
    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    // Draw sprite
    virtual void Draw(SpriteRenderer &renderer);
};

class BallObject : public GameObject
{
    public:
        // 球的状态 
        GLfloat   Radius;
        GLboolean Stuck;
        GLboolean Sticky, PassThrough;

        BallObject();
        BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite);

        glm::vec2 Move(GLfloat dt, GLuint window_width);
        void      Reset(glm::vec2 position, glm::vec2 velocity);
}; 

const glm::vec2 SIZE(60, 20);
const glm::vec2 VELOCITY(0.0f, 150.0f);

class PowerUp : public GameObject 
{
public:
    // 道具类型
    std::string Type;
    GLfloat     Duration;   
    GLboolean   Activated;
    // 构造函数
    PowerUp(std::string type, glm::vec3 color, GLfloat duration, 
            glm::vec2 position, Texture2D texture) 
        : GameObject(position, SIZE, texture, color, VELOCITY), 
          Type(type), Duration(duration), Activated() 
    { }
}; 