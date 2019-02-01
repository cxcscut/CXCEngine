#ifndef CXC_RIGIDBODYCOMPONENT_H
#define CXC_RIGIDBODYCOMPONENT_H

#include "CSceneComponent.h"
#include <memory>

namespace cxc
{
	class RigidBody3D;

	/* CRigidBodyComponent is the component that enables the Object simulate physics */
	class CXC_ENGINECORE_API CRigidBodyComponent : public CSceneComponent
	{
	public:
		CRigidBodyComponent();
		virtual ~CRigidBodyComponent();

	public:

		glm::mat4 EvaluateLocalTransform() const;

		virtual void SetLocalTransform(const glm::mat4& Transform);

	public:

		void InitializeRigidBody();
		std::shared_ptr<RigidBody3D> GetRigidBody() { return pRigidBody; }

	protected:

		std::shared_ptr<RigidBody3D> pRigidBody;
	};

}
#endif // CXC_RIGIDBODYCOMPONENT_H