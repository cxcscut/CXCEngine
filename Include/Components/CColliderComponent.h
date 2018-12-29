#ifndef CXC_COLLIDERCOMPONENT_H
#define CXC_COLLIDERCOMPONENT_H

#include "Components/CSceneComponent.h"
#include <memory>

namespace cxc
{
	class Collider3D;

	/* The CColliderComponent is the base class for all the Collider component */
	class CXC_ENGINECORE_API CColliderComponent : public CSceneComponent
	{
	public:
		CColliderComponent();
		virtual ~CColliderComponent();

	public:

		void SetCollider(std::shared_ptr<Collider3D> Collider) { pCollider = Collider; }
		std::shared_ptr<Collider3D> GetCollider() { return pCollider; }

	protected:

		std::shared_ptr<Collider3D> pCollider;
	};
}

#endif // CXC_COLLIDERCOMPONENT_H