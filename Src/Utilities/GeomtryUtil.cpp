#include "Utilities/GeometryUtil.h"
#include "Geometry/Mesh.h"
#include "Geometry/SubMesh.h"
#include "Material/Material.h"

namespace cxc
{
	GeometryUtil::GeometryUtil()
	{

	}

	GeometryUtil::~GeometryUtil()
	{

	}

	std::shared_ptr<Mesh> GeometryUtil::MakeSphere(float Radius, const glm::vec3& Center, uint32_t Segment, const glm::vec3& Color)
	{
		std::vector<glm::vec3> Vertices;
		std::vector<glm::vec3> Normals;
		std::vector<uint32_t> Indices;

		// Create vertices and normals of the sphere
		for (size_t Latitude = 0; Latitude <= Segment; ++Latitude)
		{
			float Theta = Latitude * PI / Segment;
			float sinTheta = sinf(Theta);
			float cosTheta = cosf(Theta);

			for (size_t Longtitude = 0; Longtitude <= Segment; ++Longtitude)
			{
				float Phi = Longtitude * PI / Segment;
				float sinPhi = sinf(Phi);
				float cosPhi = cosf(Phi);

				glm::vec3 Normal(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
				glm::vec3 Vertex = Normal * Radius;

				Vertices.emplace_back(Vertex);
				Normals.emplace_back(Normal);
			}
		}

		// Create the indices array of the sphere
		for (size_t Latitude = 0; Latitude < Segment; ++Latitude)
		{
			for (size_t Longtitude = 0; Longtitude < Segment; ++Longtitude)
			{
				int first = (Latitude * (Segment + 1)) + Longtitude;
				int second = first + Segment + 1;

				Indices.push_back(first);
				Indices.push_back(second);
				Indices.push_back(first + 1);

				Indices.push_back(second);
				Indices.push_back(second + 1);
				Indices.push_back(first + 1);
			}
		}

		auto pSphere = NewObject<Mesh>(Vertices, Normals);
		pSphere->Translate(Center);

		auto pSubMesh = NewObject<SubMesh>(Indices);
		auto pMaterial = NewObject<Material>();
		pMaterial->DiffuseFactor = Color;
		pSubMesh->SetSubMeshMaterial(pMaterial);
		pSphere->AddSubMesh(pSubMesh);

		return pSphere;
	}

	std::shared_ptr<Mesh> GeometryUtil::MakeBox(const glm::vec3& Center, const glm::vec3& Extent, const glm::vec3& Color)
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

		std::shared_ptr<Mesh> pBox = NewObject<Mesh>(Vertices, Normals);
		pBox->Translate(Center);
		pBox->Scale(Extent);

		auto pSubMesh = NewObject<SubMesh>(Indices);
		auto pMaterial = NewObject<Material>();
		pMaterial->DiffuseFactor = Color;
		pSubMesh->SetSubMeshMaterial(pMaterial);
		pBox->AddSubMesh(pSubMesh);

		return pBox;
	}

	glm::vec3 GeometryUtil::ComputeSurfaceNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
	{
		return glm::normalize(glm::cross(v3-v2, v1-v2));
	}
}