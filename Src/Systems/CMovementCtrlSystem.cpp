#include "Systems/CMovementCtrlSystem.h"
#include "Components/CSceneComponent.h"

namespace cxc
{
	CMovementCtrlSystem::CMovementCtrlSystem()
	{

	}

	CMovementCtrlSystem::~CMovementCtrlSystem()
	{

	}

	void CMovementCtrlSystem::Translate(std::shared_ptr<CComponent> Component, float x, float y, float z)
	{
		auto pSceneComponent = std::dynamic_pointer_cast<CSceneComponent>(Component);
		if (pSceneComponent)
		{
			auto TranslationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
			pSceneComponent->SetLocalTransform(TranslationMatrix * pSceneComponent->EvaluateLocalTransform());
		}
	}

	void CMovementCtrlSystem::RotateLocalSpace(std::shared_ptr<CComponent> Component, float x, float y, float z)
	{
		auto pSceneComponent = std::dynamic_pointer_cast<CSceneComponent>(Component);
		if (pSceneComponent)
		{
			// Rotate around x-axis
			auto RotationMatrix = glm::rotate(glm::mat4(1.0f), x, glm::vec3(1, 0, 0));

			// Rotate around y-axis
			auto RotationMatrix = glm::rotate(RotationMatrix, y, glm::vec3(0, 1, 0));

			// Rotate around z-axis
			auto RotationMatrix = glm::rotate(RotationMatrix, z, glm::vec3(0, 1, 0));

			pSceneComponent->SetLocalTransform(pSceneComponent->EvaluateLocalTransform() * RotationMatrix);
		}
	}

	void CMovementCtrlSystem::RotateWorldSpace(std::shared_ptr<CComponent> Component, float x, float y, float z)
	{
		auto pSceneComponent = std::dynamic_pointer_cast<CSceneComponent>(Component);
		if (pSceneComponent)
		{
			// Rotate around x-axis
			auto RotationMatrix = glm::rotate(glm::mat4(1.0f), x, glm::vec3(1, 0, 0));

			// Rotate around y-axis
			auto RotationMatrix = glm::rotate(RotationMatrix, y, glm::vec3(0, 1, 0));

			// Rotate around z-axis
			auto RotationMatrix = glm::rotate(RotationMatrix, z, glm::vec3(0, 1, 0));

			pSceneComponent->SetLocalTransform(RotationMatrix * pSceneComponent->EvaluateLocalTransform());
		}
	}

	void CMovementCtrlSystem::RotateWithArbitraryAxis(std::shared_ptr<CComponent> Component, 
		float pos_x, float pos_y, float pos_z, float x, float y, float z)
	{
		auto pSceneComponent = std::dynamic_pointer_cast<CSceneComponent>(Component);
		if (pSceneComponent)
		{
			// Translate to the origin
			auto TransToOrigin = glm::translate(glm::mat4(1.0f), -glm::vec3(pos_x, pos_y, pos_z));

			// Rotate around x-axis
			auto RotationMatrix = glm::rotate(glm::mat4(1.0f), x, glm::vec3(1, 0, 0));

			// Rotate around y-axis
			auto RotationMatrix = glm::rotate(RotationMatrix, y, glm::vec3(0, 1, 0));

			// Rotate around z-axis
			auto RotationMatrix = glm::rotate(RotationMatrix, z, glm::vec3(0, 0, 1));

			// Translate back
			auto TransBack = glm::translate(glm::mat4(1.0f), glm::vec3(pos_x, pos_y, pos_z));

			pSceneComponent->SetLocalTransform(TransBack * RotationMatrix * TransToOrigin * pSceneComponent->EvaluateLocalTransform());
		}
	}
}