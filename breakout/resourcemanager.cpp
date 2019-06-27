#include "resourcemanager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <stb_image/stb_image.h>

// Instantiate static vars
std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Texture2D> ResourceManager::Textures;

Shader ResourceManager::LoadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile, std::string name)
{
	Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	return Shaders[name];
}

Shader& ResourceManager::GetShader(std::string name)
{
	return Shaders[name];
}

// Loads (and generates a texture from file
Texture2D ResourceManager::LoadTexture(const GLchar* file, GLboolean alpha, std::string name)
{
	Textures[name] = loadTextureFromFile(file, alpha);
	return Textures[name];
}

Texture2D& ResourceManager::GetTexture(std::string name)
{
	return Textures[name];
}

void ResourceManager::Clear()
{
	// (Properly) delete all shaders
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
	for (auto iter : Textures)
		glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	
	try
	{
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);
		std::stringstream vShaderStream;
		std::stringstream fShaderStream;

		// Read file's buffer contents into streams
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		
		vertexShaderFile.close();
		fragmentShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

		if (gShaderFile != nullptr)
		{
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();

		}
	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER: Failed to read the shader files\n";
	}

	const GLchar* vShaderCode = vertexCode.c_str();
	const GLchar* fShaderCode = fragmentCode.c_str();
	const GLchar* gShaderCode = geometryCode.c_str();

	Shader shader;
	shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
	return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const GLchar* file, GLboolean alpha)
{
	Texture2D texture;
	if (alpha)
	{
		texture.InternalFormat = GL_RGBA;
		texture.ImageFormat = GL_RGBA;
	}

	int width;
	int height;
	int nrChannels;
	
	stbi_set_flip_vertically_on_load(true); // use stbi to flip a texture on y-axis
	unsigned char* image = stbi_load(file, &width, &height, &nrChannels, 0);
	if (image)
	{
		texture.Generate(width, height, image);
	}
	else
	{
		std::cout << "Failed to load texture\n";
	}
	
	stbi_image_free(image);
	return texture;
}