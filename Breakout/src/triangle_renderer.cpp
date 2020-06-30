#include "triangle_renderer.h"

TriangleRenderer::TriangleRenderer(Shader& shader)
{
	this->shader = shader;
	this->initRenderData();
}

TriangleRenderer::~TriangleRenderer()
{
	glDeleteVertexArrays(1, &this->triangleVAO);
}

void TriangleRenderer::Draw(glm::vec3 position, glm::vec3 size, GLfloat rotate, glm::vec3 color)
{
    this->shader.use();
    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, position);
    model = glm::scale(model, size);
    //model = glm::rotate(model, rotate, );

    this->shader.setMat4("model", model);
    this->shader.setVec3("color", color);

    glBindVertexArray(this->triangleVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
}

void TriangleRenderer::initRenderData()
{
    // ÅäÖÃ VAO/VBO
    GLuint VBO;
    GLfloat vertices[] = {
        // Î»ÖÃ     
        0.0f, 1.0f, 0.0f,
        0.5f, 0.0f, 0.0f,
        -0.5f, 0.0f, 0.0f,
    };

    glGenVertexArrays(1, &this->triangleVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->triangleVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
