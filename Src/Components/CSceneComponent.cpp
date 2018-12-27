#include "Components/CSceneComponent.h"

namespace cxc
{
	CSceneComponent::CSceneComponent():
		LocalTransformMatrix(glm::mat4(1.0f))
	{

	}

	CSceneComponent::~CSceneComponent()
	{

	}

	glm::mat4 CSceneComponent::EvaluateGlobalTransform()
	{
		glm::mat4 GlobalTransform = LocalTransformMatrix;
		auto pParentNode = ParentComponent.lock();
		while (pParentNode != nullptr)
		{
			auto ParentSceneComponent = std::dynamic_pointer_cast<CSceneComponent>(pParentNode);
			if (ParentSceneComponent)
			{
				GlobalTransform = LocalTransformMatrix * ParentSceneComponent->EvaluateLocalTransform();
			}

			pParentNode = pParentNode->GetParentComponent();
		}

		return GlobalTransform;
	}

	void  CSceneComponent::Tick(float DeltaSeconds)
	{

	}
}