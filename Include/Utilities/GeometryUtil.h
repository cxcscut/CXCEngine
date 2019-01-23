#include "Core/EngineTypes.h"

#ifndef CXC_GEOMETRYUTIL_H
#define CXC_GEOMETRYUTIL_H

namespace cxc
{
	class CSkeleton;

	class CXC_ENGINECORE_API GeometryUtil final
	{
	public:
		GeometryUtil();
		~GeometryUtil();

	public:

		static void MakeSphere(float Radius, uint32_t Segment, std::vector<glm::vec3>& OutVertices, std::vector<uint32_t>& OutIndices);
		static void MakeBox(const glm::vec3& Extent, std::vector<glm::vec3>& OutVertices, std::vector<uint32_t>& OutIndices);
		static void MakeCone(const glm::vec3& Axis, float ConeAngle, std::vector<glm::vec3>& OutVertices, std::vector<uint32_t>& OutIndices);
		static void MakeSkeleton(std::shared_ptr<CSkeleton> pSkeleton, std::vector<glm::vec3>& OutVertices, std::vector<uint32_t>& OutIndices);

	};
}

#endif // CXC_GEOMETRYUTIL_H