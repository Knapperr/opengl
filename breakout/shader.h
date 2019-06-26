#ifndef _Shader_HG_
#define _Shader_HG_

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	GLuint ID;

	Shader &Use();
	void Compile(const GLchar* vertexSource, const GLchar* fragmentSource, const GLchar* geometrySource = nullptr); 

	// Utility functions
	void    SetFloat	(const GLchar* name, GLfloat value, GLboolean useShader = false);
	void    SetInteger	(const GLchar* name, GLint value, GLboolean useShader = false);
	void    SetVector2f	(const GLchar* name, GLfloat x, GLfloat y, GLboolean useShader = false);
	void    SetVector2f	(const GLchar* name, const glm::vec2& value, GLboolean useShader = false);
	void    SetVector3f	(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);
	void    SetVector3f	(const GLchar* name, const glm::vec3& value, GLboolean useShader = false);
	void    SetVector4f	(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false);
	void    SetVector4f	(const GLchar* name, const glm::vec4& value, GLboolean useShader = false);
	void    SetMatrix4	(const GLchar* name, const glm::mat4& matrix, GLboolean useShader = false);

private:
	enum ERROR_TYPE
	{
		VERTEX,
		FRAGMENT,
		GEOMETRY,
		PROGRAM
	};

	// Checks if compilation or linking failed and if so, print the error logs
	void checkCompileErrors(GLuint object, ERROR_TYPE type);
};

#endif