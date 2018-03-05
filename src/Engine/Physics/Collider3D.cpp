#include "Collider3D.h"
#include <iostream>
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

		// When using Double version of this function, nan occurs when dMassSetTrimesh invoked, but I don't know what's going on here
		dGeomTriMeshDataBuildSingle(m_TriMeshDataID,
									&vertices.front(), 3 * sizeof(float), vertices.size(), 
									&indices.front(), indices.size(), 3 * sizeof(uint32_t));

		m_GeomID = dCreateTriMesh(space, m_TriMeshDataID, nullptr, nullptr, nullptr);
		
		dMassSetTrimesh(&m, DESITY, m_GeomID);

		dGeomSetPosition(m_GeomID,-m.c[0],-m.c[1],-m.c[2]);
		dMassTranslate(&m,-m.c[0],-m.c[1],-m.c[2]);

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