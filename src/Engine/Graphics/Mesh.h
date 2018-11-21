
#ifdef WIN32

#include "..\General\DefineTypes.h"
#include "..\Graphics\MaterialManager.h"

#else

#include "../General/DefineTypes.h"
#include "../Graphics/MaterialManager.h"

#endif

#ifndef CXC_MESH_H
#define CXC_MESH_H

namespace cxc
{
	/* Mesh is a collection of polygons that have the same material, an object can have many meshes */
	class Mesh final
	{
		friend class FBXSDKUtil;

	public:
		Mesh();
		~Mesh();

	public:

		void BindMaterial(GLuint KaLocation, GLuint KdLocation, GLuint KsLocation, GLuint TexSampler);
		void DrawMesh();

	public:

		void SetMeshMaterial(std::shared_ptr<Material> pNewMaterial) { pMaterial = pNewMaterial; }
		void SetOwnerObject(std::shared_ptr<Object3D> OwnerObject);
		std::shared_ptr<Object3D> GetOwnerObject();
		std::shared_ptr<Material> GetMeshMaterial() { return pMaterial; }

	private:

		std::weak_ptr<Object3D> pOwnerObject;

		std::vector<uint32_t> Indices;

		std::shared_ptr<Material> pMaterial;
	};
}

#endif // CXC_MESH_H