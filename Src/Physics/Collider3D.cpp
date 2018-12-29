#include "Physics/Collider3D.h"
#include "Physics/RigidBody3D.h"
#include "Geometry/Mesh.h"
#include <iostream>

namespace cxc {

	Collider3D::Collider3D():
		ColliderGeomID(0)
	{
		pOwnerRigidBody.reset();
	}

	Collider3D::~Collider3D()
	{
		dGeomDestroy(ColliderGeomID);
		pOwnerRigidBody.reset();
	}

	void Collider3D::SetColliderPosition(dReal x, dReal y, dReal z) noexcept
	{
		dGeomSetPosition(ColliderGeomID, x, y, z);
	}

	void Collider3D::SetColliderRotation(const dMatrix3& RotMatrix)
	{
		dGeomSetRotation(ColliderGeomID, RotMatrix);
	}

	std::shared_ptr<RigidBody3D> Collider3D::GetOwnerRigidBody()
	{
		if (pOwnerRigidBody.expired())
			return nullptr;
		else
			return pOwnerRigidBody.lock();
	}

	void Collider3D::BindRigidBody(std::shared_ptr<RigidBody3D> pRigidBody) noexcept
	{
		if (pRigidBody)
		{
			pOwnerRigidBody = pRigidBody;
			dGeomSetBody(ColliderGeomID, pRigidBody->GetBodyID());
		}
	}

	dSpaceID Collider3D::GetGeomSpace() const noexcept
	{
		return dGeomGetSpace(ColliderGeomID);
	}

	void Collider3D::EnableGeom() noexcept
	{
		dGeomEnable(ColliderGeomID);
	}

	void Collider3D::DisableGeom() noexcept
	{
		dGeomDisable(ColliderGeomID);
	}

	bool Collider3D::isGeomEnable() const noexcept
	{
		return dGeomIsEnabled(ColliderGeomID);
	}
}