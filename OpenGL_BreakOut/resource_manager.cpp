#include "resource_manager.h"

Shader ResourceManager::LoadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile, std::string name)
{
	return Shader("", "");
}

Shader ResourceManager::GetShader(std::string name)
{
	return Shader("", "");
}

Texture2D ResourceManager::LoadTexture(const GLchar* file, GLboolean alpha, std::string name)
{
	return Texture2D();
}

Texture2D ResourceManager::GetTexture(std::string name)
{
	return Texture2D();
}

void ResourceManager::Clear()
{
}

Shader ResourceManager::loadShaderFromFile(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile)
{
	return Shader("", "");
}

Texture2D ResourceManager::loadTextureFromFile(const GLchar* file, GLboolean alpha)
{
	return Texture2D();
}
