#include "shader.h"
#include "texture.h"

#ifndef TRIANGLE_RENDERER_H
#define TRIANGLE_RENDERER_H


class TriangleRenderer
{
public:
    TriangleRenderer(Shader& shader);
    ~TriangleRenderer();

    void Draw(glm::vec3 position,
        glm::vec3 size = glm::vec3(1, 1, 1), GLfloat rotate = 0.0f,
        glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f));
private:
    Shader shader;
    GLuint triangleVAO;

    void initRenderData();
};

#endif // !TRIANGLE_RENDERER_H
