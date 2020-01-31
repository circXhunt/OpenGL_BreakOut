#include "resource_manager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// Instantiate static variables
std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;

Shader ResourceManager::LoadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile, std::string name)
{
	Shader shader(vShaderFile, gShaderFile, fShaderFile);
	Shaders[name] = shader;
	return shader;
}

Shader ResourceManager::LoadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, std::string name)
{
	Shader shader(vShaderFile, fShaderFile);
	Shaders[name] = shader;
	return shader;
}

Shader ResourceManager::GetShader(std::string name)
{
	return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const GLchar* file, GLboolean alpha, std::string name)
{
	Textures[name] = loadTextureFromFile(file, alpha);
	return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
	return Textures[name];
}

void ResourceManager::Clear()
{    // (Properly) delete all shaders	
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
	// (Properly) delete all textures
	for (auto iter : Textures)
		glDeleteTextures(1, &iter.second.ID);
}

Texture2D ResourceManager::loadTextureFromFile(const GLchar* file, GLboolean alpha)
{
	Texture2D texture;
	texture.Internal_Format = GL_RGBA;
	texture.Image_Format = GL_RGBA;
	int width, height, nrComponents;
	unsigned char* data = stbi_load(file, &width, &height, &nrComponents, 0);
	if (data)
	{
		texture.Generate(width, height, data);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << file << std::endl;
		stbi_image_free(data);
	}
	return texture;
}
