#include "Collider3D.h"

namespace cxc {

	Collider3D::Collider3D():
		m_GeomID(0),m_TriMeshDataID(0)
	{

	}

	Collider3D::~Collider3D()
	{
		if (m_TriMeshDataID)
			destroyTriMeshGeom();
	}

	void Collider3D::createTriMeshGeom(dSpaceID space, const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices) noexcept
	{
		m_TriMeshDataID = dGeomTriMeshDataCreate();
		dGeomTriMeshDataBuildDouble(m_TriMeshDataID, &vertices.front(), sizeof(glm::vec3), vertices.size(), &indices.front(), indices.size(), 3 * sizeof(uint32_t));

		m_GeomID = dCreateTriMesh(space, m_TriMeshDataID, nullptr, nullptr, nullptr);
	}

	void Collider3D::destroyTriMeshGeom() noexcept
	{
		dGeomTriMeshDataDestroy(m_TriMeshDataID);
	}

	void Collider3D::setGeomPosition(dReal x, dReal y, dReal z) noexcept
	{
		dGeomSetPosition(m_GeomID, x, y, z);
	}

	void Collider3D::associateRigidBody(dBodyID body) noexcept
	{
		dGeomSetBody(m_GeomID,body);
	}

	void Collider3D::setGeomRotation(const glm::mat4 & rot) noexcept
	{
		dMatrix3 _rot = { rot[0][0], rot[1][0], rot[2][0], rot[3][0],
			rot[0][1], rot[1][1], rot[2][1], rot[3][1],
			rot[0][2], rot[1][2], rot[2][2], rot[3][2] };

		if (m_GeomID)
			dGeomSetRotation(m_GeomID, _rot);
	}

	glm::vec3 Collider3D::getGeomPosition() const noexcept
	{
		glm::vec3 ret;

		if (m_GeomID)
		{
			const dReal* pos = dGeomGetPosition(m_GeomID);
			ret.x = pos[0]; ret.y = pos[1]; ret.z = pos[2];
		}

		return ret;
	}

	glm::mat3 Collider3D::getGeomRotation() const noexcept
	{
		glm::mat3 ret;

		if (m_GeomID)
		{
			const dReal *rot = dGeomGetRotation(m_GeomID);
			ret = glm::transpose(glm::mat3({
				rot[0],rot[1],rot[2],
				rot[3],rot[4],rot[5],
				rot[6],rot[7],rot[8]
			}));
		}

		return ret;
	}

	dSpaceID Collider3D::getGeomSpace() const noexcept
	{
		return dGeomGetSpace(m_GeomID);
	}

	int Collider3D::getGeomClass() const noexcept
	{
		return dGeomGetClass(m_GeomID);
	}

	void Collider3D::enableGeom() noexcept
	{
		dGeomEnable(m_GeomID);
	}

	void Collider3D::disableGeom() noexcept
	{
		dGeomDisable(m_GeomID);
	}

	bool Collider3D::isGeomEnable() const noexcept
	{
		return dGeomIsEnabled(m_GeomID);
	}

}