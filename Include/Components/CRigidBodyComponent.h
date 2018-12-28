#ifndef CXC_RIGIDBODYCOMPONENT_H
#define CXC_RIGIDBODYCOMPONENT_H

#include "CSceneComponent.h"
#include "Physics/RigidBody3D.h"

namespace cxc
{
	/* CRigidBodyComponent is the component that enables the Object simulate physics */
	class CXC_ENGINECORE_API CRigidBodyComponent : public CSceneComponent, public RigidBody3D
	{
	public:
		CRigidBodyComponent();
		virtual ~CRigidBodyComponent();

	public:

		glm::mat4 EvaluateLocalTransform() const;

		virtual void SetLocalTransform(const glm::mat4& Transform);

	public:

		void PhysicsTick(float DeltaSeconds);

	public:

		void InitializeRigidBody();
	};

}
#endif // CXC_RIGIDBODYCOMPONENT_H