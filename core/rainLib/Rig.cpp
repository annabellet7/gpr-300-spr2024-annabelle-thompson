#include "Rig.h"

Rig::Rig()
{
	mJoints[0].mParentIndex = -1;
	mJoints[0].mLocalPose.mPosition = glm::vec3(0.0f, 1.0f, 0.0f);
	mJoints[0].mLocalPose.mRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	mJoints[0].mLocalPose.mScale = 1.0f;
	mJoints[0].mGlobalPose.mPosition = mJoints[0].mLocalPose.mPosition;
	mJoints[0].mGlobalPose.mRotation = mJoints[0].mLocalPose.mRotation;
	mJoints[0].mGlobalPose.mScale = mJoints[0].mLocalPose.mScale;

	mJoints[1].mParentIndex = 0;
	mJoints[1].mLocalPose.mPosition = glm::vec3(0.0f, 1.5f, 0.0f);
	mJoints[1].mLocalPose.mRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	mJoints[1].mLocalPose.mScale = 0.5f;
	mJoints[1].mGlobalPose.mPosition = mJoints[1].mLocalPose.mPosition;
	mJoints[1].mGlobalPose.mRotation = mJoints[1].mLocalPose.mRotation;
	mJoints[1].mGlobalPose.mScale = mJoints[1].mLocalPose.mScale;

	mJoints[2].mParentIndex = 0;
	mJoints[2].mLocalPose.mPosition = glm::vec3(1.5f, 0.0f, 0.0f);
	mJoints[2].mLocalPose.mRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	mJoints[2].mLocalPose.mScale = 0.5f;
	mJoints[2].mGlobalPose.mPosition = mJoints[2].mLocalPose.mPosition;
	mJoints[2].mGlobalPose.mRotation = mJoints[2].mLocalPose.mRotation;
	mJoints[2].mGlobalPose.mScale = mJoints[2].mLocalPose.mScale;

	mJoints[3].mParentIndex = 2;
	mJoints[3].mLocalPose.mPosition = glm::vec3(1.0f, 0.0f, 0.0f);
	mJoints[3].mLocalPose.mRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	mJoints[3].mLocalPose.mScale = 0.9f;
	mJoints[3].mGlobalPose.mPosition = mJoints[3].mLocalPose.mPosition;
	mJoints[3].mGlobalPose.mRotation = mJoints[3].mLocalPose.mRotation;
	mJoints[3].mGlobalPose.mScale = mJoints[3].mLocalPose.mScale;

	mJoints[4].mParentIndex = 3;
	mJoints[4].mLocalPose.mPosition = glm::vec3(1.0f, 0.0f, 0.0f);
	mJoints[4].mLocalPose.mRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	mJoints[4].mLocalPose.mScale = 0.8f;
	mJoints[4].mGlobalPose.mPosition = mJoints[4].mLocalPose.mPosition;
	mJoints[4].mGlobalPose.mRotation = mJoints[4].mLocalPose.mRotation;
	mJoints[4].mGlobalPose.mScale = mJoints[4].mLocalPose.mScale;

	mJoints[5].mParentIndex = 0;
	mJoints[5].mLocalPose.mPosition = glm::vec3(-1.5f, 0.0f, 0.0f);
	mJoints[5].mLocalPose.mRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	mJoints[5].mLocalPose.mScale = 0.5f;
	mJoints[5].mGlobalPose.mPosition = mJoints[5].mLocalPose.mPosition;
	mJoints[5].mGlobalPose.mRotation = mJoints[5].mLocalPose.mRotation;
	mJoints[5].mGlobalPose.mScale = mJoints[5].mLocalPose.mScale;

	mJoints[6].mParentIndex = 5;
	mJoints[6].mLocalPose.mPosition = glm::vec3(-1.0f, 0.0f, 0.0f);
	mJoints[6].mLocalPose.mRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	mJoints[6].mLocalPose.mScale = 0.9f;
	mJoints[6].mGlobalPose.mPosition = mJoints[6].mLocalPose.mPosition;
	mJoints[6].mGlobalPose.mRotation = mJoints[6].mLocalPose.mRotation;
	mJoints[6].mGlobalPose.mScale = mJoints[6].mLocalPose.mScale;

	mJoints[7].mParentIndex = 6;
	mJoints[7].mLocalPose.mPosition = glm::vec3(-1.0f, 0.0f, 0.0f);
	mJoints[7].mLocalPose.mRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	mJoints[7].mLocalPose.mScale = 0.8f;
	mJoints[7].mGlobalPose.mPosition = mJoints[7].mLocalPose.mPosition;
	mJoints[7].mGlobalPose.mRotation = mJoints[7].mLocalPose.mRotation;
	mJoints[7].mGlobalPose.mScale = mJoints[7].mLocalPose.mScale;
}

void Rig::SolveFK()
{
	for (int i = 0; i < mNumJoints; i++)
	{
		if (mJoints[i].mParentIndex == -1)
		{
			mJoints[i].mGlobalPose.mScale = mJoints[i].mLocalPose.mScale;
			mJoints[i].mGlobalPose.mRotation = mJoints[i].mLocalPose.mRotation;
			mJoints[i].mGlobalPose.mPosition = mJoints[i].mLocalPose.mPosition;
		}
		else
		{
			mJoints[i].mGlobalPose.mScale = mJoints[mJoints[i].mParentIndex].mGlobalPose.mScale * mJoints[i].mLocalPose.mScale;
			mJoints[i].mGlobalPose.mRotation = mJoints[mJoints[i].mParentIndex].mGlobalPose.mRotation + mJoints[i].mLocalPose.mRotation;
		
			glm::vec3 dir = glm::quat(glm::radians(mJoints[mJoints[i].mParentIndex].mGlobalPose.mRotation))* mJoints[i].mLocalPose.mPosition;
			mJoints[i].mGlobalPose.mPosition = dir + mJoints[mJoints[i].mParentIndex].mGlobalPose.mPosition;
		}
	}
}