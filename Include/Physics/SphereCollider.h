#ifndef CXC_SPHERECOLLIDER_H
#define CXC_SPHERECOLLIDER_H

#include "Physics/Collider3D.h"

namespace cxc
{
	/* SphereCollider is the Collider3D that uses sphere to handle collision */
	class CXC_ENGINECORE_API SphereCollider : public Collider3D
	{
	public:
		SphereCollider();
		virtual ~SphereCollider();

	public:
		
		void ModifySphereRadius(float NewRadius);
		void CreateSphereColliderGeom(dSpaceID SpaceID, float Radius);

		float GetSphereColliderRadius() const;
	};
}

#endif // CXC_SPHERECOLLIDER_H