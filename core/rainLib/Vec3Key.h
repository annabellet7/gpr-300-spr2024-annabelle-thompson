#pragma once

#include <glm/glm.hpp>

class Vec3Key
{
public:
	float mTime;
	glm::vec3 mValue; 

	Vec3Key();
	Vec3Key(float time, glm::vec3 value); 
};
