#include "Utilities/GeometryUtil.h"
#include "Geometry/Mesh.h"
#include "Geometry/SubMesh.h"
#include "Material/Material.h"
#include "Animation/Skeleton.h"
#include "Animation/LinkBone.h"

namespace cxc
{
	GeometryUtil::GeometryUtil()
	{

	}

	GeometryUtil::~GeometryUtil()
	{

	}

	void  GeometryUtil::MakeSphere(float Radius, uint32_t Segment, std::vector<glm::vec3>& OutVertices, std::vector<uint32_t>& OutIndices)
	{
		OutVertices.clear();
		OutIndices.clear();

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

				OutVertices.emplace_back(Vertex);
			}
		}

		// Create the indices array of the sphere
		for (size_t Latitude = 0; Latitude < Segment; ++Latitude)
		{
			for (size_t Longtitude = 0; Longtitude < Segment; ++Longtitude)
			{
				int first = (Latitude * (Segment + 1)) + Longtitude;
				int second = first + Segment + 1;

				OutIndices.push_back(first);
				OutIndices.push_back(second);
				OutIndices.push_back(first + 1);

				OutIndices.push_back(second);
				OutIndices.push_back(second + 1);
				OutIndices.push_back(first + 1);
			}
		}
	}

	void GeometryUtil::MakeBox(const glm::vec3& Extent, std::vector<glm::vec3>& OutVertices, std::vector<uint32_t>& OutIndices)
	{
		OutVertices.clear();
		OutIndices.clear();

		// Create the vertices of the box
		OutVertices = 
		{
			{1.0, -1.0, 1.0}, {1.0, 1.0, 1.0}, {-1.0, 1.0 , 1.0}, {-1.0, -1.0 , 1.0},
			{1.0, -1.0, -1.0},{1.0, 1.0, -1.0}, {-1.0, 1.0, -1.0}, {-1.0, -1.0, -1.0}
		};

		// Create the indices array of the box
		OutIndices = 
		{
			0, 1, 2, 0, 2, 3,
			6, 5, 4, 4, 7, 6,
			0, 3, 7, 0, 7, 4,
			2, 1, 5, 2 ,5 ,6,
			1, 0, 4, 1, 4, 5,
			3, 2, 6, 3, 6, 7
		};

		// Extenting
		for (auto& Vertex : OutVertices)
			Vertex *= Extent;
	}

	void GeometryUtil::MakeSkeleton(std::shared_ptr<CSkeleton> pSkeleton, std::vector<glm::vec3>& OutVertices, std::vector<uint32_t>& OutIndices)
	{
		OutVertices.clear();
		OutIndices.clear();

		size_t IndiceCount = 0;

		if (pSkeleton)
		{
			auto BoneCount = pSkeleton->GetBoneCount();
			for (size_t BoneIndex = 0; BoneIndex < BoneCount; ++BoneIndex)
			{
				auto pBone = pSkeleton->GetBone(BoneIndex);

				OutVertices.emplace_back(pBone->GetStartPos());
				OutVertices.emplace_back(pBone->GetEndPos());

				OutIndices.push_back(IndiceCount++);
				OutIndices.push_back(IndiceCount++);
			}
		}
	}
}