#include "Core/EngineTypes.h"
#include "Material/MaterialManager.h"

#ifndef CXC_SUBMESH_H
#define CXC_SUBMESH_H

namespace cxc
{
	class SubMeshRenderer;
	enum class eShadingMode : uint16_t;

	/* SubMesh is a collection of polygons that have the same material, an object can have many meshes */
	class CXC_ENGINECORE_API SubMesh
	{
		friend class FBXSDKUtil;

	public:
		SubMesh();
		SubMesh(const std::vector<uint32_t>& Indices);
		virtual ~SubMesh();

	public:

		virtual void BindMaterial(GLuint ProgramID, const MaterialDiffuseSubroutineInfo& DiffuseModelInfo ,std::vector<GLuint>& SubroutinesIndicesFS);
	
	public:

		virtual void DrawSubMesh();
		void AllocateSubMeshEBO();
		void ReleaseSubMeshEBO();

	public:

		void SetShadingMode(eShadingMode Mode) { ShadingMode = Mode; }
		void SetSubMeshMaterial(std::shared_ptr<Material> pNewMaterial) { pMaterial = pNewMaterial; }
		void SetOwnerObject(std::shared_ptr<Mesh> OwnerObject);

		eShadingMode GetShadingMode() const { return ShadingMode; }
		GLuint GetSubMeshEBO() const { return SubMeshEBO; }
		std::shared_ptr<Mesh> GetOwnerMesh();
		std::shared_ptr<Material> GetSubMeshMaterial() { return pMaterial; }
		const std::vector<uint32_t>& GetSubMeshVertexIndices() const { return Indices; }

	protected:

		// EBO of the mesh
		GLuint SubMeshEBO;

		// Weak pointer back to the Mesh that owns the mesh
		std::weak_ptr<Mesh> pOwnerMesh;

		// Vertex indices of the mesh
		std::vector<uint32_t> Indices;

		// Material of the mesh
		std::shared_ptr<Material> pMaterial;

		// Shading mode of the submesh
		eShadingMode ShadingMode;
	};
}

#endif // CXC_SUBMESH_H