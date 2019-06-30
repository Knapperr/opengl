#ifndef _postprocessor_HG_
#define _postprocessor_HG_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "spriterenderer.h"
#include "shader.h"

class PostProcessor
{
public:
	Shader PostProcessingShader;
	Texture2D Texture;
	unsigned int Width;
	unsigned int Height;
	bool Confuse;
	bool Chaos;
	bool Shake;

	PostProcessor(Shader shader, unsigned int width, unsigned int height);
	void BeginRender(void);
	void EndRender(void);
	void Render(float time);

private:
	unsigned int m_MSFBO; // Multisampled FBO
	unsigned int m_FBO;   // Regular FBO used for blitting MS color-buffer to texture
	unsigned int m_RBO;   // Used for multisampled color buffer
	unsigned int m_VAO;

	// Init quad for rendering postprocessing texture
	void initRenderData(void);
};

#endif