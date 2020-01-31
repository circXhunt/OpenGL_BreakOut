#include "game_level.h"

GameLevel::GameLevel()
{
}

void GameLevel::Load(const GLchar* file, GLuint levelWidth, GLuint levelHeight)
{
	this->Bricks.clear();

	GLuint tileCode;
	GameLevel level;
	std::string line;
	std::ifstream fstream(file);
	std::vector<std::vector<GLuint>> tileData;
	if (fstream)
	{
		while (std::getline(fstream, line))
		{
			std::istringstream sstream(line);
			std::vector<GLuint> row;
			while (sstream >> tileCode)
			{
				row.push_back(tileCode);
			}
			tileData.push_back(row);
		}
		if (tileData.size() > 0)
			this->init(tileData, levelWidth, levelHeight);
	}
}

void GameLevel::Draw(SpriteRenderer& renderer)
{
}

GLboolean GameLevel::IsCompleted()
{
	return GLboolean();
}

void GameLevel::init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight)
{
	// 计算每个维度的大小
	GLuint height = tileData.size();
	GLuint width = tileData[0].size();
}
