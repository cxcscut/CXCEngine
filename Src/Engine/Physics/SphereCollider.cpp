#include "Physics/SphereCollider.h"

namespace cxc
{
	SphereCollider::SphereCollider()
	{

	}

	SphereCollider::~SphereCollider()
	{

	}

	void SphereCollider::ModifySphereRadius(float NewRadius)
	{
		if (ColliderGeomID)
		{
			dGeomSphereSetRadius(ColliderGeomID, NewRadius);
		}
	}

	void SphereCollider::CreateSphereColliderGeom(dSpaceID SpaceID, float Radius)
	{
		ColliderGeomID = dCreateSphere(SpaceID, Radius);
		dGeomSetData(ColliderGeomID, reinterpret_cast<void*>(this));
	}

	float SphereCollider::GetSphereColliderRadius() const
	{
		return static_cast<float>(dGeomSphereGetRadius(ColliderGeomID));
	}
}