#include "postprocessor.h"

#include <iostream>

PostProcessor::PostProcessor(Shader shader, unsigned int width, unsigned int height)
	: PostProcessingShader(shader)
	, Texture()
	, Width(width)
	, Height(height)
	, Confuse(GL_FALSE)
	, Chaos(GL_FALSE)
	, Shake(GL_FALSE)
{
	// Init renderbuffer/framebuffer
	glGenFramebuffers(1, &this->m_MSFBO);
	glGenFramebuffers(1, &this->m_FBO);
	glGenRenderbuffers(1, &this->m_RBO);

	// Get the max samples 
	int maxSamples;
	glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);

	// Init renderbuffer storage with a multisampled color buffer (don't need a depth/stencil buffer)
	glBindFramebuffer(GL_FRAMEBUFFER, this->m_MSFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->m_RBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, maxSamples, GL_RGB, width, height); // Allocate storage for render buffer object
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->m_RBO); // Attach MS render buffer object to framebuffer
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::POSTPROCESSOR: Failed to init MSFBO\n";
	}

	// Also init the FBO/texture to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
	glBindFramebuffer(GL_FRAMEBUFFER, this->m_FBO);
	this->Texture.Generate(width, height, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->Texture.ID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::POSTPROCESSOR: Failed to init FBO\n";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Init render data and uniforms
	this->initRenderData();
	this->PostProcessingShader.SetInteger("scene", 0, GL_TRUE);
	float offset = 1.0f / 300.0f;
	float offsets[9][2] = {
		{ -offset,	offset  },  // top-left
		{  0.0f,	offset  },  // top-center
		{  offset,	offset  },  // top-right
		{ -offset,	0.0f    },  // center-left
		{  0.0f,	0.0f    },  // center-center
		{  offset,	0.0f    },  // center-right
		{ -offset,	-offset  },  // bottom-left
		{  0.0f,	-offset  },  // bottom-center
		{  offset,	-offset  }   // bottom-right
	};
	glUniform2fv(glGetUniformLocation(this->PostProcessingShader.ID, "offsets"), 9, (GLfloat*)offsets);
	
	int edge_kernel[9] = {
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1
	};
	glUniform1iv(glGetUniformLocation(this->PostProcessingShader.ID, "edge_kernel"), 9, edge_kernel);

	float blur_kernel[9] = {
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16
	};
	glUniform1fv(glGetUniformLocation(this->PostProcessingShader.ID, "blur_kernel"), 9, blur_kernel);
}

void PostProcessor::BeginRender(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->m_MSFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::EndRender(void)
{
	// Now resolve multisampled color-buffer into intermediate FBO to store to texture
	glBindFramebuffer(GL_READ_FRAMEBUFFER, this->m_MSFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->m_FBO);
	glBlitFramebuffer(0, 0, this->Width, this->Height, 0, 0, this->Width, this->Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Binds both READ and WRITE framebuffer to default frame buffer
}

void PostProcessor::Render(float time)
{
	// Set uniforms/options
	this->PostProcessingShader.Use();
	this->PostProcessingShader.SetFloat("time", time);
	this->PostProcessingShader.SetInteger("confuse", this->Confuse);
	this->PostProcessingShader.SetInteger("chaos", this->Chaos);
	this->PostProcessingShader.SetInteger("shake", this->Shake);

	// Render texture quad
	glActiveTexture(GL_TEXTURE0);
	this->Texture.Bind();
	glBindVertexArray(this->m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void PostProcessor::initRenderData(void)
{
	// Configure VAO/VBO
	GLuint VBO;
	GLfloat vertices[] = {
		// Pos		    // Tex
		-1.0f, -1.0f,   0.0f, 0.0f,
		 1.0f,  1.0f,   1.0f, 1.0f,
		-1.0f,  1.0f,   0.0f, 1.0f,
					    
		-1.0f, -1.0f,   0.0f, 0.0f,
		 1.0f, -1.0f,   1.0f, 0.0f,
		 1.0f,  1.0f,   1.0f, 1.0f
	};

	glGenVertexArrays(1, &this->m_VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->m_VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}