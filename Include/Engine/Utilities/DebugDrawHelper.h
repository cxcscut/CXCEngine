#ifndef CXC_DEBUGDRAWHELPER_H
#define CXC_DEBUGDRAWHELPER_H

#include "Singleton.inl"
#include "Core/EngineCore.h"
#include "Core/EngineTypes.h"

#define DrawDebugSphere(Pos, Radius, Segment, MeshColor, Persistence) (DebugDrawHelper::GetInstance()->AddDebugSphereShape(Pos, Radius, Segment, MeshColor, Persistence))
#define DrawDebugCube(Pos, Extents, MeshColor, Persistence) (DebugDrawHelper::GetInstance()->AddDebugCubeShape(Pos, Extents, MeshColor, Persistence))
#define FlushDebugMeshse() (DebugDrawHelper::GetInstance()->RemoveDebugShapes())

namespace cxc
{
	class GeometryUtil;

	class CXC_ENGINECORE_API DebugDrawHelper final : public Singleton<DebugDrawHelper>
	{
	public:
		friend class Singleton<DebugDrawHelper>;

		DebugDrawHelper();
		~DebugDrawHelper();

	public:
	
		void RemoveDebugShapes();

	public:

		void AddDebugSphereShape(const glm::vec3& Pos, float Radius, uint32_t Segment, const glm::vec3& Color, float Persistence);
		void AddDebugCubeShape(const glm::vec3 &Pos, const glm::vec3 Extents, const glm::vec3& Color, float Persistence);

	};
}

#endif // CXC_DEBUGDRAWHELPER_H