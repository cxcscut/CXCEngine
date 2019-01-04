#include "Core/EngineTypes.h"

#ifndef CXC_GEOMETRYUTIL_H
#define CXC_GEOMETRYUTIL_H

namespace cxc
{
	class DebugMesh;

	class CXC_ENGINECORE_API GeometryUtil final
	{
	public:
		GeometryUtil();
		~GeometryUtil();

	public:

		static std::shared_ptr<DebugMesh> MakeSphere(float Radius, const glm::vec3& Center, uint32_t Segment, const glm::vec3& Color = {1,0,0});
		static std::shared_ptr<DebugMesh> MakeBox(const glm::vec3& Center, const glm::vec3& Extent, const glm::vec3& Color = { 1,0,0 });
		static std::shared_ptr<DebugMesh> MakeCone(const glm::vec3& Center, const glm::vec3& Axis, float ConeAngle, const glm::vec3& Color = { 1,0,0 });

	};
}

#endif // CXC_GEOMETRYUTIL_H