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

		void SetDebugMeshColor(const glm::vec3& Color) { DebugMeshColor = Color; }
		glm::vec3 GetDebugMeshColor() const { return DebugMeshColor; }

	protected:

		// Color of the debug mesh
		glm::vec3 DebugMeshColor;
	};
}

#endif // CXC_DEBUGMESH_H