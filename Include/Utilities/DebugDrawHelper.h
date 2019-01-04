#ifndef CXC_DEBUGDRAWHELPER_H
#define CXC_DEBUGDRAWHELPER_H

#include "Singleton.inl"
#include "Core/EngineCore.h"
#include "Core/EngineTypes.h"

#define DrawDebugSphere(Pos, Radius, Segment, MeshColor) (DebugDrawHelper::GetInstance()->AddDebugSphereMesh(Pos, Radius, Segment, MeshColor))
#define DrawDebugCube(Pos, Extents, MeshColor) (DebugDrawHelper::GetInstance()->AddDebugCubeMesh(Pos, Extents, MeshColor))
#define FlushDebugMeshse() (DebugDrawHelper::GetInstance()->RemoveDebugMeshes())

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
	
		void RemoveDebugMeshes();

	public:

		void AddDebugSphereMesh(const glm::vec3& Pos, float Radius, uint32_t Segment, const glm::vec3& Color);
		void AddDebugCubeMesh(const glm::vec3 &Pos, const glm::vec3 Extents, const glm::vec3& Color);

	};
}

#endif // CXC_DEBUGDRAWHELPER_H