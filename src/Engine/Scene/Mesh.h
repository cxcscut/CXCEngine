
#ifdef WIN32

#include "..\General\DefineTypes.h"
#include "..\Material\MaterialManager.h"

#else

#include "../General/DefineTypes.h"
#include "../Material/MaterialManager.h"

#endif

#ifndef CXC_MESH_H
#define CXC_MESH_H

namespace cxc
{
	class MeshRender;

	/* Mesh is a collection of polygons that have the same material, an object can have many meshes */
	class Mesh final
	{
		friend class FBXSDKUtil;

	public:
		Mesh();
		~Mesh();

	public:

		void BindMaterial(GLuint ProgramID, const MaterialDiffuseSubroutineInfo& DiffuseModelInfo ,std::vector<GLuint>& SubroutinesIndicesFS);
	
	public:

		void DrawMesh();
		void AllocateMeshEBO();
		void ReleaseMeshEBO();

	public:

		void SetMeshMaterial(std::shared_ptr<Material> pNewMaterial) { pMaterial = pNewMaterial; }
		void SetOwnerObject(std::shared_ptr<Object3D> OwnerObject);
		void BindMeshRender(std::shared_ptr<MeshRender> pNewRender) { pMeshRender = pNewRender; }

		GLuint GetMeshEBO() const { return MeshEBO; }
		std::shared_ptr<MeshRender> GetMeshRender() { return pMeshRender; }
		std::shared_ptr<Object3D> GetOwnerObject();
		std::shared_ptr<Material> GetMeshMaterial() { return pMaterial; }
		const std::vector<uint32_t>& GetMeshVertexIndices() const { return Indices; }

	private:

		// EBO of the mesh
		GLuint MeshEBO;

		// Weak pointer back to the Object3D that owns the mesh
		std::weak_ptr<Object3D> pOwnerObject;

		// Vertex indices of the mesh
		std::vector<uint32_t> Indices;

		// Material of the mesh
		std::shared_ptr<Material> pMaterial;

		// MeshRender that renders the mesh
		std::shared_ptr<MeshRender> pMeshRender;
	};
}

#endif // CXC_MESH_H