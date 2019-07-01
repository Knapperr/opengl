#ifndef _ballobject_HG_
#define _ballobject_HG_

#include "gameobject.h"
#include "powerup.h"

class BallObject : public GameObject
{
public:
	float Radius;
	bool Stuck;
	bool Sticky;
	bool PassThrough;

	BallObject();
	BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

	glm::vec2	Move(float deltaTime, unsigned int window_width);
	void		Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif