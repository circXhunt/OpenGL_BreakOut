#include <vector>
#include "game_object.h"

#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

class GameLevel
{
public:
	GameLevel();

	std::vector<GameObject> Bricks;


    // ���ļ��м��عؿ�
    void Load(const GLchar* file, GLuint levelWidth, GLuint levelHeight);
    // ��Ⱦ�ؿ�
    void Draw(SpriteRenderer& renderer);
    // ���һ���ؿ��Ƿ������ (���зǼ�Ӳ�Ĵ�ש�����ݻ�)
    GLboolean IsCompleted();
private:
    // ��ש�����ݳ�ʼ���ؿ�
    void init(std::vector<std::vector<GLuint>> tileData,
        GLuint levelWidth, GLuint levelHeight);
};


#endif // !GAME_LEVEL_H
