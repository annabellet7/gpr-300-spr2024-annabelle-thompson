#include "Vec3Key.h"

Vec3Key::Vec3Key()
{
	mTime = 0;
	mValue = glm::vec3(0.0, 0.0, 0.0);
}

Vec3Key::Vec3Key(float time, glm::vec3 value)
{
	mTime = time;
	mValue = value;
}

