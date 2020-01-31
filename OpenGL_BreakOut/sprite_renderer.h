#include "shader.h"
#include "texture.h"
#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H


class SpriteRenderer
{
public:
    SpriteRenderer(Shader& shader);
    ~SpriteRenderer();

    void DrawSprite(const Texture2D& texture, glm::vec2 position,
        glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f,
        glm::vec3 color = glm::vec3(1.0f));
private:
    Shader shader;
    GLuint quadVAO;

    void initRenderData();
};
#endif // SPRITE_RENDERER_H
