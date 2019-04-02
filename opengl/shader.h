#pragma once 
#include <glad/glad.h>
#include <string>

class Shader {
public:
	unsigned int ID;
	// build the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	// use/activate shader
	void use();
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
private:
	void checkCompileErrors(unsigned int shader, std::string type);
};