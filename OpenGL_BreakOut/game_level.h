#include <vector>
#include "game_object.h"
#include "resource_manager.h"

#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

class GameLevel
{
public:
    GameLevel(std::string levelName):Bricks(), LevelName(levelName) {
        std::string path = "resources/textures/background/" + LevelName + ".jpg";
        ResourceManager::LoadTexture(path.c_str(), GL_FALSE, LevelName);
    }

	std::vector<GameObject> Bricks;
    std::string LevelName;

    Texture2D GetBackground() {
        return ResourceManager::GetTexture(LevelName);
    }
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
