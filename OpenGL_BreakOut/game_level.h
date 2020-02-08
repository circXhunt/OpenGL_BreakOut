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
    // 从文件中加载关卡
    void Load(const GLchar* file, GLuint levelWidth, GLuint levelHeight);
    // 渲染关卡
    void Draw(SpriteRenderer& renderer);
    // 检查一个关卡是否已完成 (所有非坚硬的瓷砖均被摧毁)
    GLboolean IsCompleted();
private:
    // 由砖块数据初始化关卡
    void init(std::vector<std::vector<GLuint>> tileData,
        GLuint levelWidth, GLuint levelHeight);
};


#endif // !GAME_LEVEL_H
