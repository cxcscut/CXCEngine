#include "Actor/CActor.h"

namespace cxc
{

	CActor::CActor() :
		CObject()
	{

	}

	CActor::CActor(const std::string& Name, const std::string& Tag):
		CObject(Name, Tag)
	{

	}

	CActor::~CActor()
	{

	}

	void CActor::Translate(const glm::vec3 &TranslationVector)
	{
		auto TranslationMatrix = glm::translate(glm::mat4(1.0f), TranslationVector);
		TransformMatrix = TranslationMatrix * TransformMatrix;
	}

	void CActor::RotateWorldSpace(const glm::vec3 &RotationAxisWorldSpace, float Degree)
	{
		auto RotMatrix = glm::rotate(glm::mat4(1.0f), Degree, RotationAxisWorldSpace);
		TransformMatrix = RotMatrix * TransformMatrix;
	}

	void CActor::RotateLocalSpace(const glm::vec3 &RotationAxisLocalSpace, float Degree)
	{
		auto RotMatrix = glm::rotate(glm::mat4(1.0f), Degree, RotationAxisLocalSpace);
		TransformMatrix = TransformMatrix * TransformMatrix;
	}

	void CActor::Scale(const glm::vec3& ScalingVector)
	{
		auto ScalingMatrix = glm::scale(glm::mat4(1.0f), ScalingVector);
		TransformMatrix = TransformMatrix * ScalingMatrix;
	}

	void CActor::RotateWithArbitraryAxis(const glm::vec3 &Position, const glm::vec3 &RotationAxis, float Degree)
	{
		// In the global coordinate system, matrix multiplication sequence should be reversed
		// while in the local coordinate system, matrix multiplication sequence should not be reversed
		// here we use local coordinate system in which transformation matrix should be left-multiplied

		// Perform rotation on root node
		// Move to world origin
		glm::mat4 TransToOrigin = glm::translate(glm::mat4(1.0f), -Position);
		glm::mat4 RotMatrix = glm::rotate(glm::mat4(1.0f), Degree, RotationAxis);
		glm::mat4 TransBack = glm::translate(glm::mat4(1.0f), Position);
		auto ChainMatrix = TransBack * RotMatrix * TransToOrigin;

		TransformMatrix = ChainMatrix * TransformMatrix;
	}

	glm::mat4 CActor::GetModelMatrix() const
	{
		return TransformMatrix;
	}
}