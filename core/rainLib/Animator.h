#pragma once

#include "AnimationClip.h"
#include <glm/glm.hpp>

class Animator
{
public:
	Animator();
	~Animator();

	float animatorTime(float deltaTime); 
	glm::vec3 getPosition(float deltaTime);
	glm::vec3 getRotation(float deltaTime);
	glm::vec3 getScale(float deltaTime);
	float inverseLerp(float timeOne, float timeTwo, float inbetween);
	AnimationClip* getClip();

	bool mIsPlaying;
	bool mIsLooping;
	float mPlaybackTime;
	float mPlaybackSpeed;

private:
	AnimationClip* mClip;
};