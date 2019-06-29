#include "particlegenerator.h"

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount)
	: m_shader(shader)
	, m_texture(texture)
	, m_amount(amount)
{
	this->init();
}

void ParticleGenerator::Update(float deltaTime, GameObject& object, unsigned int newParticles, glm::vec2 offset)
{
	// Add new particles
	for (unsigned int i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->m_particles[unusedParticle], object, offset);
	}
	// Update all particles
	for (unsigned int i = 0; i < this->m_amount; ++i)
	{
		Particle &p = this->m_particles[i];
		p.Life -= deltaTime;
		if (p.Life > 0.0f)
		{
			p.Position -= p.Velocity * deltaTime;
			p.Color.a -= deltaTime * 2.5;
		}
	}
}

void ParticleGenerator::Draw(void)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Use additive blending to give a glow effect
	this->m_shader.Use(); 
	for (Particle particle : this->m_particles)
	{
		if (particle.Life > 0.0f)
		{
			this->m_shader.SetVector2f("offset", particle.Position);
			this->m_shader.SetVector4f("color", particle.Color);
			this->m_texture.Bind();
			glBindVertexArray(this->m_VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	// Reset to the default blending mode -- remember opengl is a big state machine
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
	unsigned int VBO;
	float particleQuad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &this->m_VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particleQuad), particleQuad, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Create this->amount default particle instances
	for (unsigned int i = 0; i < this->m_amount; ++i)
	{
		this->m_particles.push_back(Particle());
	}
}

// Stores the index of the last particle used (for quick access to next dead particle)
unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle(void)
{
	// search from last used particle, this will usually return almost instantly
	for (unsigned int i = lastUsedParticle; i < this->m_amount; ++i)
	{
		if (this->m_particles[i].Life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	// otherwise do a linear search
	for (unsigned int i = 0; i < lastUsedParticle; ++i)
	{
		if (this->m_particles[i].Life <= 0.0f)
		{
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one 
	// (note that if it repeatdely hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset)
{
	float random = ((rand() % 100) - 50) / 10.0f;
	float rColor = 0.5 + ((rand() % 100) / 100.0f);
	particle.Position = object.Position + random + offset;
	particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.Life = 1.0f;
	particle.Velocity = object.Velocity * 0.1f;
}