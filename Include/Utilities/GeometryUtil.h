#include "General/DefineTypes.h"

#ifndef CXC_GEOMETRYUTIL_H
#define CXC_GEOMETRYUTIL_H

namespace cxc
{
	class Mesh;

	class GeometryUtil final
	{
	public:
		GeometryUtil();
		~GeometryUtil();

	public:

		static std::shared_ptr<Mesh> MakeSphere(float Radius, const glm::vec3& Center, uint32_t Segment, const glm::vec3& Color = {1,0,0});
		static std::shared_ptr<Mesh> MakeBox(const glm::vec3& Center, const glm::vec3& Extent, const glm::vec3& Color = { 1,0,0 });
		static std::shared_ptr<Mesh> MakeCone(const glm::vec3& Center, const glm::vec3& Axis, float ConeAngle, const glm::vec3& Color = { 1,0,0 });

	public:

		static glm::vec3 ComputeSurfaceNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
	};
}

#endif // CXC_GEOMETRYUTIL_H