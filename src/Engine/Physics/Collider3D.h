#ifndef CXC_PHYSICS_COLLIDER_H
#define CXC_PHYSICS_COLLIDER_H

#include "ode\ode.h"
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

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
		void setGeomRotation(const glm::mat4 & rot) noexcept;

		glm::vec3 getGeomPosition() const noexcept;
		glm::mat4 getGeomRotation() const noexcept;

		dSpaceID getGeomSpace() const noexcept;
		int getGeomClass() const noexcept;

		void enableGeom() noexcept;
		void disableGeom() noexcept;
		bool isGeomEnable() const noexcept;

	private:

		dTriMeshDataID m_TriMeshDataID;
		// Only single geom supported for each RigidBody3D
		dGeomID m_GeomID;

	};
}

#endif // CXC_PHYSICS_COLLIDER_H
