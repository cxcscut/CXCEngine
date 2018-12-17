#include "Utilities/GeometryUtil.h"
#include "Scene/Object3D.h"
#include "Material/Material.h"

namespace cxc
{
	GeometryUtil::GeometryUtil()
	{

	}

	GeometryUtil::~GeometryUtil()
	{

	}

	std::shared_ptr<Object3D> GeometryUtil::MakeSphere(float Radius, const glm::vec3& Center, uint32_t Segment, const glm::vec3& Color)
	{

	}

	std::shared_ptr<Object3D> GeometryUtil::MakeBox(const glm::vec3& Center, const glm::vec3& Extent, const glm::vec3& Color)
	{
		// Create the vertices of the box
		std::vector<glm::vec3> Vertices = 
		{
			{1.0, 1.0, 1.0}, {-1.0, 1.0, 1.0}, {-1.0, 1.0, -1.0}, {1.0, 1.0, -1.0},
			{1.0, -1.0, 1.0}, {-1.0, -1.0, 1.0}, {-1.0, -1.0 , -1.0}, {1.0, -1.0, -1.0},
			{-1.0, 1.0, 1.0}, {-1.0, -1.0, 1.0}, {-1.0, -1.0, -1.0}, {-1.0, 1.0, -1.0},
			{1.0, 1.0, 1.0}, {1.0, -1.0, 1.0}, {1.0, -1.0, -1.0}, {1.0, 1.0, -1.0},
			{1.0, -1.0, 1.0}, {-1.0, -1.0, 1.0}, {-1.0, 1.0, 1.0}, {1.0, 1.0, 1.0},
			{1.0, -1.0, -1.0}, {-1.0, -1.0, -1.0}, {-1.0, 1.0, -1.0}, {1.0, 1.0, -1.0}
		};

		// Create the normals of the box
		std::vector<glm::vec3> Normals;
		for (size_t PolygonIndex = 0; PolygonIndex < 6; ++PolygonIndex)
		{
		
			glm::vec3 v1 = Vertices[PolygonIndex * 4];
			glm::vec3 v2 = Vertices[PolygonIndex * 4 + 1];
			glm::vec3 v3 = Vertices[PolygonIndex * 4 + 2];

			// Compute surface normal
			auto SurfaceNormal = ComputeSurfaceNormal(v1, v2, v3);

			for (size_t VertexIndex = 0; VertexIndex < 4; ++VertexIndex)
			{
				Normals.emplace_back(SurfaceNormal);
			}
		}

		// Create the indices array of the box
		std::vector<uint32_t> Indices = 
		{
			0, 1, 2, 0, 2, 3,
			4, 5, 6, 4, 6, 7,
			8, 9, 10, 8, 10, 11,
			12, 13, 14, 12, 14, 15,
			16, 17, 18, 16, 18, 19,
			20, 21, 22, 20, 22, 23
		};

		std::shared_ptr<Object3D> pBox = std::make_shared<Object3D>(Vertices, Normals);
		pBox->Translate(Center);
		pBox->Scale(Extent);

		auto pMesh = std::make_shared<Mesh>(Indices);
		auto pMaterial = std::make_shared<Material>();
		pMaterial->DiffuseFactor = Color;
		pMesh->SetMeshMaterial(pMaterial);
		pBox->AddMesh(pMesh);
	}

	std::shared_ptr<Object3D> GeometryUtil::MakeCone(const glm::vec3& Center, const glm::vec3& Direction, float ConeAngle, const glm::vec3& Color)
	{

	}

	glm::vec3 GeometryUtil::ComputeSurfaceNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
	{
		return glm::normalize(glm::cross(v3-v2, v1-v2));
	}
}