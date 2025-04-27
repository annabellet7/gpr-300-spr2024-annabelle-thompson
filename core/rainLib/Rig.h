#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <glm/gtc/quaternion.hpp>

struct JointPose
{
	glm::vec3 mRotation;
	glm::vec3 mPosition;
	float mScale;
};

struct Joint
{
	int mParentIndex;
	JointPose mLocalPose;
	JointPose mGlobalPose;
};

struct Rig
{
	Joint mJoints[8];
	int mNumJoints = 8;

	Rig();
	void SolveFK();
};