#include "Animator.h"
#include <glm/gtc/quaternion.hpp>
#include "Vec3Key.h"

Animator::Animator()
{
	mIsPlaying = true;
	mPlaybackSpeed = 1.0;
	mIsLooping = true;
	mPlaybackTime = 0;
	mClip = new AnimationClip();
}

Animator::~Animator()
{
	delete mClip;
}

float Animator::animatorTime(float deltaTime)
{
	if (mIsPlaying)
	{
		mPlaybackTime += deltaTime;
	}

	if (mIsLooping && mPlaybackTime > mClip->mDuration)
	{
		mPlaybackTime = 0.0f;
	}
	
	if (!mIsLooping && mPlaybackTime > mClip->mDuration)
	{
		mPlaybackTime = mClip->mDuration;
	}

	return mPlaybackTime;
}

glm::vec3 Animator::getPosition(float deltaTime)
{
	Vec3Key next;
	Vec3Key prev;

	animatorTime(deltaTime);

	if (mClip->mPositionKeys.size() == 0)
	{
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}

	if (mPlaybackTime == 0)
	{
		return mClip->mPositionKeys[0].mValue;
	}

	if (mPlaybackTime >= mClip->mPositionKeys[mClip->mPositionKeys.size() - 1].mTime)
	{
		return mClip->mPositionKeys[mClip->mPositionKeys.size() - 1].mValue;
	}

		for (int i = 0; i < mClip->mPositionKeys.size(); i++)
		{
			if (mClip->mPositionKeys[i].mTime >= mPlaybackTime)
			{
				next.mTime = mClip->mPositionKeys[i].mTime;
				next.mValue = mClip->mPositionKeys[i].mValue;
				prev.mTime = mClip->mPositionKeys[i - 1].mTime;
				prev.mValue = mClip->mPositionKeys[i - 1].mValue;
				break;
			}
		}

	float t = inverseLerp(prev.mTime, next.mTime, mPlaybackTime);
	glm::vec3 pos = glm::mix(prev.mValue, next.mValue, t);

	return pos;
}

glm::vec3 Animator::getRotation(float deltaTime)
{
	Vec3Key next;
	Vec3Key prev;

	animatorTime(deltaTime);

	if (mClip->mRotationKeys.size() == 0)
	{
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}

	if (mPlaybackTime == 0)
	{
		return glm::radians(mClip->mRotationKeys[0].mValue);
	}

	if (mPlaybackTime >= mClip->mRotationKeys[mClip->mRotationKeys.size()-1].mTime)
	{
		return glm::radians(mClip->mRotationKeys[mClip->mRotationKeys.size()-1].mValue);
	}

	for (int i = 0; i < mClip->mRotationKeys.size(); i++)
	{
		if (mClip->mRotationKeys[i].mTime >= mPlaybackTime)
		{
			next.mTime = mClip->mRotationKeys[i].mTime;
			next.mValue = mClip->mRotationKeys[i].mValue;
			prev.mTime = mClip->mRotationKeys[i-1].mTime;
			prev.mValue = mClip->mRotationKeys[i-1].mValue;
			break;
		}
	}

	float t = inverseLerp(prev.mTime, next.mTime, mPlaybackTime);

	glm::vec3 radNext = glm::radians(next.mValue);
	glm::vec3 redPrec = glm::radians(prev.mValue);
	glm::quat rotQuatNext = glm::quat(radNext);
	glm::quat rotQuatPrev = glm::quat(redPrec);

	glm::quat quatLerp = glm::mix(rotQuatPrev, rotQuatNext, t);

	glm::vec3 rotation = glm::eulerAngles(quatLerp);

	return rotation;
}

glm::vec3 Animator::getScale(float deltaTime)
{
	Vec3Key next;
	Vec3Key prev;

	animatorTime(deltaTime);

	if (mClip->mScaleKeys.size() == 0)
	{
		return glm::vec3(1.0f, 1.0f, 1.0f);
	}

	if (mPlaybackTime == 0)
	{
		return mClip->mScaleKeys[0].mValue;
	}

	if (mPlaybackTime >= mClip->mScaleKeys[mClip->mScaleKeys.size() - 1].mTime)
	{
		return mClip->mScaleKeys[mClip->mScaleKeys.size() - 1].mValue;
	}

	for (int i = 0; i < mClip->mScaleKeys.size(); i++)
	{
		if (mClip->mScaleKeys[i].mTime >= mPlaybackTime)
		{
			next.mTime = mClip->mScaleKeys[i].mTime;
			next.mValue = mClip->mScaleKeys[i].mValue;
			prev.mTime = mClip->mScaleKeys[i - 1].mTime;
			prev.mValue = mClip->mScaleKeys[i - 1].mValue;
			break;
		}
	}

	float t = inverseLerp(prev.mTime, next.mTime, mPlaybackTime);
	glm::vec3 scale = glm::mix(prev.mValue, next.mValue, t);

	return scale;
}

float Animator::inverseLerp(float timeOne, float timeTwo, float inbetween)
{
	double t = ((inbetween - timeOne) / (timeTwo - timeOne));
	t = glm::clamp(t, 0.0, 1.0);
	return float(t);
}

AnimationClip* Animator::getClip()
{
	return mClip;
}
