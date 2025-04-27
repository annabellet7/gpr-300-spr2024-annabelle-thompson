#pragma once

#include "Vec3Key.h"
#include <vector>

class AnimationClip
{
public:
	AnimationClip();

	void addPositionFrame(); 
	void addRotationFrame(); 
	void addScaleFrame(); 

	void removePositionFrame();
	void removeRotationFrame();
	void removeScaleFrame();

	void setPosKey(float time, glm::vec3 value, int i);
	void setRotationKey(float time, glm::vec3 value, int i);
	void setScaleKey(float time, glm::vec3 value, int i);

	void setDuration(float duration);

	float mDuration, mLastFrame;
	std::vector<Vec3Key> mPositionKeys;
	std::vector<Vec3Key> mRotationKeys;
	std::vector<Vec3Key> mScaleKeys;
private:
};