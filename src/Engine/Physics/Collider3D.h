#ifndef CXC_PHYSICS_COLLIDER_H
#define CXC_PHYSICS_COLLIDER_H

#define MAX_CONTACT_NUM 16
#define DESITY 0.0005f
#define dDOUBLE

#include <vector>

#ifdef WIN32

#include "ode\ode.h"
#include <glm/gtc/matrix_transform.hpp>

#else

#include "ode/ode.h"
#include <glm/gtc/matrix_transform.hpp>

#endif // WIN32

namespace cxc {

	class Collider3D
	{

	public:

		Collider3D();
		virtual ~Collider3D();

		// Geometry
	public:

		void createTriMeshGeom(dSpaceID space, const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices) noexcept;
		void destroyTriMeshGeom() noexcept;

		void setGeomPosition(dReal x, dReal y, dReal z) noexcept;

		dSpaceID getGeomSpace() const noexcept;
		int getGeomClass() const noexcept;

		void enableGeom() noexcept;
		void disableGeom() noexcept;
		bool isGeomEnable() const noexcept;

		void associateRigidBody(dBodyID body) noexcept;

		dTriMeshDataID getTriMeshID() const noexcept { return m_TriMeshDataID; };
		dGeomID getGeomID() const noexcept { return m_GeomID; };

		dMass GetMass() const noexcept { return m; };

	private:

		dTriMeshDataID m_TriMeshDataID;
		// Only single geom supported for each RigidBody3D
		dGeomID m_GeomID;

		dMass m;

	};
}

#endif // CXC_PHYSICS_COLLIDER_H
