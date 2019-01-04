#include "Utilities/GeometryUtil.h"
#include "Geometry/Mesh.h"
#include "Geometry/SubMesh.h"
#include "Material/Material.h"
#include "Geometry/DebugMesh.h"

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
		std::vector<uint32_t> Indices;

		// Create vertices and normals of the sphere
		for (size_t Latitude = 0; Latitude <= Segment; ++Latitude)
		{
			float Theta = Latitude * 2 * PI / Segment;
			float sinTheta = sinf(Theta);
			float cosTheta = cosf(Theta);

			for (size_t Longtitude = 0; Longtitude <= Segment; ++Longtitude)
			{
				float Phi = Longtitude * 2 * PI / Segment;
				float sinPhi = sinf(Phi);
				float cosPhi = cosf(Phi);

				glm::vec3 Normal(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
				glm::vec3 Vertex = Normal * Radius;

				Vertices.emplace_back(Vertex);
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

		auto pSphere = NewObject<DebugMesh>(Vertices);
		pSphere->SetDebugMeshColor(Color);
		pSphere->Translate(Center);

		auto pSubMesh = NewObject<SubMesh>(Indices);
		pSubMesh->SetShadingMode(eShadingMode::WireframeMode);

		pSphere->AddSubMesh(pSubMesh);

		return pSphere;
	}

	std::shared_ptr<Mesh> GeometryUtil::MakeBox(const glm::vec3& Center, const glm::vec3& Extent, const glm::vec3& Color)
	{
		// Create the vertices of the box
		std::vector<glm::vec3> Vertices = 
		{
			{1.0, -1.0, 1.0}, {1.0, 1.0, 1.0}, {-1.0, 1.0 , 1.0}, {-1.0, -1.0 , 1.0},
			{1.0, -1.0, -1.0},{1.0, 1.0, -1.0}, {-1.0, 1.0, -1.0}, {-1.0, -1.0, -1.0}
		};

		// Create the indices array of the box
		std::vector<uint32_t> Indices = 
		{
			0, 1, 2, 0, 2, 3,
			6, 5, 4, 4, 7, 6,
			0, 3, 7, 0, 7, 4,
			2, 1, 5, 2 ,5 ,6,
			1, 0, 4, 1, 4, 5,
			3, 2, 6, 3, 6, 7
		};

		std::shared_ptr<DebugMesh> pBox = NewObject<DebugMesh>(Vertices);
		pBox->SetDebugMeshColor(Color);
		pBox->Translate(Center);
		pBox->Scale(Extent);

		auto pSubMesh = NewObject<SubMesh>(Indices);
		pSubMesh->SetShadingMode(eShadingMode::WireframeMode);
		pBox->AddSubMesh(pSubMesh);

		return pBox;
	}
}