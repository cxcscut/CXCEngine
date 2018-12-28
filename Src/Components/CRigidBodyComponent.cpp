#include "Components/CRigidBodyComponent.h"

namespace cxc
{
	CRigidBodyComponent::CRigidBodyComponent()
	{

	}

	CRigidBodyComponent::~CRigidBodyComponent()
	{

	}

	void CRigidBodyComponent::PhysicsTick(float DeltaSeconds)
	{
		
	}

	void CRigidBodyComponent::InitializeRigidBody()
	{

	}

	glm::mat4 CRigidBodyComponent::EvaluateLocalTransform() const
	{
		return glm::mat4(1.0f);
	}

	void CRigidBodyComponent::SetLocalTransform(const glm::mat4& Transform)
	{

	}
}