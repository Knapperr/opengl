#ifndef _particlegenerator_HG_
#define _particlegenerator_HG_

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "texture.h"
#include "gameobject.h"
#include <vector>

struct Particle {
	glm::vec2	Position;
	glm::vec2	Velocity;
	glm::vec4	Color;
	float		Life;

	Particle()
		: Position(0.0f)
		, Velocity(0.0f)
		, Color(1.0f)
		, Life(0.0f) {}
};

class ParticleGenerator
{
public:
	ParticleGenerator(Shader shader, Texture2D texture, unsigned int amount);

	void Update(float deltaTime, GameObject &object, unsigned int newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
	void Draw(void);

private:
	std::vector<Particle> m_particles;
	unsigned int m_amount;
	Shader m_shader;
	Texture2D m_texture;
	unsigned int m_VAO;

	void init(void);
	unsigned int firstUnusedParticle(); // the first particle index thats currently unused e.g Life <= 0.0f or 0 if no particle is currently active
	void respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
}; 


#endif