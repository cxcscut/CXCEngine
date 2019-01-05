#ifndef CXC_DEBUGMESH_H
#define CXC_DEBUGMESH_H

#include "Geometry/Mesh.h"

namespace cxc
{

	class CXC_ENGINECORE_API DebugMesh : public Mesh
	{
	public:
		DebugMesh(std::vector<glm::vec3>& Vertices);
		DebugMesh();
		virtual ~DebugMesh();

	public:

		virtual void AllocateBuffers() noexcept override;
		virtual void ReleaseBuffers() noexcept override;

	public:

		void MarkPendingKill() { bPendingKill = true; }
		void SetPersistance(float Time) { Persistence = Time; }
		void SetDebugMeshColor(const glm::vec3& Color) { DebugMeshColor = Color; }

		bool IsPendingKill() const { return bPendingKill; }
		float GetPersistence() const { return Persistence; }
		glm::vec3 GetDebugMeshColor() const { return DebugMeshColor; }

	protected:

		// Whether to release the mesh on next rendering tick
		// When flushing the debug meshes buffer, it will be set true
		bool bPendingKill;

		// The time of the persistence
		float Persistence;

		// Color of the debug mesh
		glm::vec3 DebugMeshColor;
	};
}

#endif // CXC_DEBUGMESH_H