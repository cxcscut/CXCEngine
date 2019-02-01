#include "Physics/BoxCollider.h"

namespace cxc
{
	BoxCollider::BoxCollider()
	{

	}

	BoxCollider::~BoxCollider()
	{

	}

	void BoxCollider::CreateBoxColliderGeom(dSpaceID SpaceID, const glm::vec3& Extents)
	{
		ColliderGeomID = dCreateBox(SpaceID, Extents.x * 2, Extents.y * 2, Extents.z * 2);
		dGeomSetData(ColliderGeomID, reinterpret_cast<void*>(this));
	}

	void BoxCollider::ModifyBoxColliderExtents(const glm::vec3& NewExtents)
	{
		dGeomBoxSetLengths(ColliderGeomID, NewExtents.x * 2, NewExtents.y * 2, NewExtents.z * 2);
	}

	glm::vec3 BoxCollider::GetBoxColliderExtents() const
	{
		dVector3 Extents;
		dGeomBoxGetLengths(ColliderGeomID, Extents);

		return glm::vec3(Extents[0], Extents[1], Extents[2]);
	}
}