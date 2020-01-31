/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "game.h"
#include "sprite_renderer.h"

#include "triangle_renderer.h"
#include "resource_manager.h"

SpriteRenderer* Renderer;

TriangleRenderer* tRenderer;
GLenum glCheckError_(const char* file, int line);

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

Game::Game(GLuint width, GLuint height)
	: State(GameState::GAME_ACTIVE), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{

}

void Game::Init()
{
	auto shader = ResourceManager::LoadShader("resources/shaders/sprite.vs", "resources/shaders/sprite.fs", "sprite");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width),
		static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	shader.use();

	shader.setInt("image", 0);
	shader.setMat4("projection", projection);
	Renderer = new SpriteRenderer(shader);
	ResourceManager::LoadTexture("resources/textures/awesomeface.png", true, "texture");

	//Shader shader("resources/shaders/triangle.vs", "resources/shaders/triangle.fs");
	//glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width),
	//    static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	//shader.use();
	//shader.setMat4("projection", projection);
	//tRenderer = new TriangleRenderer(shader);
}

void Game::Update(GLfloat dt)
{

}


void Game::ProcessInput(GLfloat dt)
{

}

void Game::Render()
{
	auto texture = ResourceManager::GetTexture("texture");

	Renderer->DrawSprite(texture,
		glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	glCheckError();
}


