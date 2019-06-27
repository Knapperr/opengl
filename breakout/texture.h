#ifndef _texture_HG_
#define _texture_HG_

#include <glad/glad.h>

class Texture2D
{
public:
	// id of texture object
	GLuint ID;
	// dimensions in pixels
	GLuint Width;
	GLuint Height;
	// format of the texture object and format of loaded image
	GLuint InternalFormat;
	GLuint ImageFormat;
	// texture configurations
	// t & s axis
	GLuint WrapS;
	GLuint WrapT;
	GLuint FilterMin;
	GLuint FilterMax;

	Texture2D();
	void Generate(GLuint width, GLuint height, unsigned char* data);
	void Bind() const;
};

#endif