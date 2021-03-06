#ifndef _powerup_HG_
#define _powerup_HG_

#include "gameobject.h"

const glm::vec2 POWER_UP_SIZE(60, 20);
const glm::vec2 VELOCITY(0.0f, 150.0f);

class PowerUp : public GameObject
{
public:
	// PowerUp State
	std::string Type;
	float Duration;
	bool Activated;

	PowerUp(std::string type, glm::vec3 color, float duration, glm::vec2 position, Texture2D texture)
		: GameObject(position, POWER_UP_SIZE, texture, color, VELOCITY)
		, Type(type)
		, Duration(duration)
		, Activated()
	{
	}
};

#endif