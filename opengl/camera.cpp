#include "camera.hpp"

#include <vector>

Camera::Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) 
	: _front(glm::vec3(0.0f, 0.0f, -1.0f))
	, 