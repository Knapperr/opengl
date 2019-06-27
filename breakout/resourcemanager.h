#ifndef _resourcemanager_HG_
#define _resourcemanager_HG_

#include <glad/glad.h>

#include <map>
#include <string>

#include "texture.h"
#include "shader.h"

// A static singleton ResourceManager class that hosts
// several functions to load Textures and Shaders. Each loaded
// texture and or shader is also stored for future reference by 
// handles. All func and resources are static
class ResourceManager
{
public:
	static std::map<std::string, Shader> Shaders;
	static std::map<std::string, Texture2D> Textures;
	
	// Loads (and generates) a shader program from file loading vertex, fragment, geometry shader's source code.
	static Shader LoadShader(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile, std::string name);
	static Shader& GetShader(std::string name);

	// Loads (and generates a texture from file
	static Texture2D LoadTexture(const GLchar* file, GLboolean alpha, std::string name);
	static Texture2D& GetTexture(std::string name);

	// properly de-allocate resources
	static void Clear();

private:
	ResourceManager() {} // make this private so its a singleton
	static Shader loadShaderFromFile(const GLchar* vShaderFile, const GLchar* fShaderFile, const GLchar* gShaderFile = nullptr);
	static Texture2D loadTextureFromFile(const GLchar* file, GLboolean alpha);
};

#endif