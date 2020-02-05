/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "game_object.h"
#include "convert.h"

GameObject::GameObject()
	: Position(0, 0), Size(1, 1), Color(1.0f), Rotation(0.0f), Sprite(), IsSolid(false), Destroyed(false), Collision(nullptr) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, std::string type, glm::vec3 color, glm::vec2 velocity)
	: Position(pos), Size(size), Color(color), Rotation(0.0f), Type(type), Sprite(sprite), IsSolid(false), Destroyed(false), Collision(nullptr) { }

GameObject::~GameObject()
{

}

void GameObject::Draw(SpriteRenderer& renderer)
{
	renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

void GameObject::Update(float dt)
{
	if (Collision)
	{
		this->Position = Convert::ph2pi(*this);
	}
}
