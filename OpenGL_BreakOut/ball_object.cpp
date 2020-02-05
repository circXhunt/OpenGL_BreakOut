#include "ball_object.h"

BallObject::BallObject()
	: GameObject(), Radius(12.5f), Stuck(GL_TRUE), Sticky(GL_FALSE), PassThrough(GL_FALSE) { }

BallObject::BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite)
	: GameObject(pos, glm::vec2(radius * 2, radius * 2), sprite, "Ball", glm::vec3(1.0f), velocity), Radius(radius), Stuck(GL_TRUE), Sticky(GL_FALSE), PassThrough(GL_FALSE) { }


glm::vec2 BallObject::Move(GLfloat dt, GLuint window_width)
{
	if (!this->Stuck)
	{

	}
	return this->Position;
}

void BallObject::Reset(glm::vec2 position, glm::vec2 velocity)
{
	this->Position = position;
	this->Stuck = true;
	this->Sticky = GL_FALSE;
	this->PassThrough = GL_FALSE;
}

void BallObject::Update(float dt)
{
	GameObject::Update(dt);
}

