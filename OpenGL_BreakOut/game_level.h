#include <vector>
#include "game_object.h"
#include "resource_manager.h"
#include "physics.h"

#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

class GameLevel
{
public:
	GameLevel() :Bricks() {}

	std::vector<GameObject> Bricks;


	// ���ļ��м��عؿ�
	void Load(const GLchar* file, Physics& physics, GLuint levelWidth, GLuint levelHeight);
	// ��Ⱦ�ؿ�
	void Draw(SpriteRenderer& renderer);
	// ���һ���ؿ��Ƿ������ (���зǼ�Ӳ�Ĵ�ש�����ݻ�)
	GLboolean IsCompleted();
private:
	// ��ש�����ݳ�ʼ���ؿ�
	void init(std::vector<std::vector<GLuint>> tileData, Physics& physics,
		GLuint levelWidth, GLuint levelHeight);
};


#endif // !GAME_LEVEL_H
