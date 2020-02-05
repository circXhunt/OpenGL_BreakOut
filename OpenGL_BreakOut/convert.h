#include <glm/glm.hpp>
#include "game_object.h"

#ifndef CONVERT_H
#define CONVERT_H

class Convert {
public:

	static glm::vec2 ph2pi(const GameObject& obj) {
		auto phPos = obj.Collision->GetPosition();
		glm::vec2 pi(phPos.x - obj.Size.x / 2, phPos.y - obj.Size.y / 2);
		return pi;
	}

	static b2Vec2 pi2ph(const GameObject& obj) {
		auto piPos = obj.Position;
		b2Vec2 ph(piPos.x + obj.Size.x / 2, piPos.y + obj.Size.y / 2);
		return ph;
	}
};

#endif // !CONVERTE_H
