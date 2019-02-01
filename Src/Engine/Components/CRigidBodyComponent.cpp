#include "Components/CRigidBodyComponent.h"
#include "Physics/RigidBody3D.h"
#include "World/World.h"

namespace cxc
{
	CRigidBodyComponent::CRigidBodyComponent()
	{

	}

	CRigidBodyComponent::~CRigidBodyComponent()
	{

	}

	void CRigidBodyComponent::InitializeRigidBody()
	{
		auto pWorld = World::GetInstance();
		auto pPhysicalWorld = pWorld->GetPhysicalWorld();
		assert(pPhysicalWorld != nullptr);
		pRigidBody = NewObject<RigidBody3D>();
		pRigidBody->CreateRigidBody(pPhysicalWorld->GetWorldID());
	}

	glm::mat4 CRigidBodyComponent::EvaluateLocalTransform() const
	{
		return pRigidBody->GetModelMatrix();
	}

	void CRigidBodyComponent::SetLocalTransform(const glm::mat4& Transform)
	{
		// Position
		pRigidBody->setPosition(Transform[3][0], Transform[3][1], Transform[3][2]);

		// Rotation
		pRigidBody->setRotation(glm::mat3(Transform));
	}
}