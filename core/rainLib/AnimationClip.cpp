#include "AnimationClip.h"

AnimationClip::AnimationClip()
{
	mDuration = 0;
	mLastFrame = 0;

	Vec3Key key;
	mPositionKeys.push_back(key);
	mRotationKeys.push_back(key);

	key.mValue = glm::vec3(1.0f, 1.0f, 1.0f);

	mScaleKeys.push_back(key);
}

void AnimationClip::addPositionFrame()
{
	Vec3Key newKey;

	if (mPositionKeys.size() == 0)
	{
		newKey.mTime = 0.0f;
		newKey.mValue = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	else
	{
		newKey.mTime = mPositionKeys[mPositionKeys.size() - 1].mTime;
		newKey.mTime = 1.0 + newKey.mTime;
		newKey.mValue = mPositionKeys[mPositionKeys.size() - 1].mValue;
	}

	mPositionKeys.push_back(newKey);

	if (newKey.mTime > mLastFrame)
	{
		mLastFrame = newKey.mTime;
	}

	if (newKey.mTime > mDuration)
	{
		mDuration = newKey.mTime;
	}
}

void AnimationClip::addRotationFrame()
{
	Vec3Key newKey;

	if (mRotationKeys.size() == 0)
	{
		newKey.mTime = 0.0f;
		newKey.mValue = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	else
	{
		newKey.mTime = mRotationKeys[mRotationKeys.size() - 1].mTime;
		newKey.mTime = 1.0 + newKey.mTime;
		newKey.mValue = mRotationKeys[mRotationKeys.size() - 1].mValue;

	}

	mRotationKeys.push_back(newKey);

	if (newKey.mTime > mLastFrame)
	{
		mLastFrame = newKey.mTime;
	}

	if (newKey.mTime > mDuration)
	{
		mDuration = newKey.mTime;
	}
}

void AnimationClip::addScaleFrame()
{
	Vec3Key newKey;

	if (mScaleKeys.size() == 0)
	{
		newKey.mTime = 0.0f;
		newKey.mValue = glm::vec3(1.0f, 1.0f, 1.0f);
	}
	else
	{
		newKey.mTime = mScaleKeys[mScaleKeys.size() - 1].mTime;
		newKey.mTime = 1.0 + newKey.mTime;
		newKey.mValue = mScaleKeys[mScaleKeys.size() - 1].mValue;


	}
	
	mScaleKeys.push_back(newKey);

	if (newKey.mTime > mLastFrame)
	{
		mLastFrame = newKey.mTime;
	}

	if (newKey.mTime > mDuration)
	{
		mDuration = newKey.mTime;
	}
}

void AnimationClip::removePositionFrame()
{
	if (mPositionKeys.size() != 0)
	{
		mPositionKeys.pop_back();
	}
}

void AnimationClip::removeRotationFrame()
{
	if (mRotationKeys.size() != 0)
	{
		mRotationKeys.pop_back();
	}
}

void AnimationClip::removeScaleFrame()
{
	if (mScaleKeys.size() != 0)
	{
		mScaleKeys.pop_back();
	}
}

void AnimationClip::setPosKey(float time, glm::vec3 value, int i)
{
	mPositionKeys[i].mTime = time;
	mPositionKeys[i].mValue = value;
}

void AnimationClip::setRotationKey(float time, glm::vec3 value, int i)
{
	mRotationKeys[i].mTime = time;
	mRotationKeys[i].mValue = value;
}

void AnimationClip::setScaleKey(float time, glm::vec3 value, int i)
{
	mScaleKeys[i].mTime = time;
	mScaleKeys[i].mValue = value;
}

void AnimationClip::setDuration(float duration)
{
	if (duration >= mLastFrame)
	{
		mDuration = duration;
	}
}
