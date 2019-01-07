#include "Core/EngineTypes.h"

#include "ode/ode.h"
#include "Physics/RigidBody3D.h"
#include "Rendering/RendererManager.h"

#ifndef CXC_DRAWOBJECT_H
#define CXC_DRAWOBJECT_H

#define CXC_NORMAL_FACTOR 0.2f
#define CXC_DIFFUSE_FACTOR (1.0f - CXC_NORMAL_FACTOR)

namespace cxc {

	class ShadowRenderer;
	class MaterialManager;
	class TextureManager;
	class Material;
	class RendringPipeline;
	class World;
	class SubMesh;

	enum class Location : GLuint 
	{
		VERTEX_LOCATION = 0,
		TEXTURE_LOCATION = 1,
		NORMAL_LOCATION = 2,
		COLOR_LOCATION = 3,
		NUM_OF_LOCATION = 4
	};

	class CXC_ENGINECORE_API CXCRect3D
	{
		
	public:

		CXCRect3D(const glm::vec3 &_max, const glm::vec3 &_min);
		CXCRect3D();
		~CXCRect3D();

		CXCRect3D(const CXCRect3D &other);
		CXCRect3D &operator=(const CXCRect3D &other);
		
		bool isContain(const CXCRect3D &rhs) const noexcept;
		bool isIntersected(const CXCRect3D &other) const noexcept;

	public:
		glm::vec3 max, min;

	};

	using VertexIndexPacket = struct VertexIndexPacket {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoords;

		VertexIndexPacket()
			:position(glm::vec3(0.0f, 0.0f, 0.0f)),
			normal(glm::vec3(0.0f, 0.0f, 0.0f)),
			texcoords(glm::vec2(0.0f, 0.0f))
		{}

		VertexIndexPacket(const glm::vec3 &pos, const glm::vec3 &_normal, const glm::vec2 &uv)
		{
			position = pos;
			normal = _normal;
			texcoords = uv;
		}

		// Overload operator < to use in the std::map
		bool operator<(const VertexIndexPacket &that) const {
			return memcmp((void*)this, (void*)&that, sizeof(VertexIndexPacket)) > 0;
		};
	};

	class CXC_ENGINECORE_API Mesh : public std::enable_shared_from_this<Mesh>
	{

	public:

		friend class SceneManager;
		friend class OctreeNode;
		friend class FBXSDKUtil;
		friend class SubMesh;
		friend class AnimContext;
		friend class CActor;
		friend class CActor;
		friend class CPawn;

		Mesh();
		Mesh(const std::string& Name);
		Mesh(std::vector<glm::vec3>& Vertices, std::vector<glm::vec3>& Normals);
		Mesh(std::vector<glm::vec3>& Vertices,
			std::vector<glm::vec3>& Normals,
			std::vector<glm::vec2>& UVs,
			std::vector<uint32_t>& Indices);
		virtual ~Mesh();

		Mesh(const std::string& Name, const std::string &filename, const std::string &_tag = "", GLboolean _enable = GL_TRUE);

	public:

		std::string GetMeshName() const { return MeshName; }
		glm::vec3 GetPivot() const noexcept;
		uint32_t GetSubMeshCount() const noexcept { return SubMeshes.size(); }
		void SetPivot(const glm::vec3& NewPivot) noexcept { Pivot = NewPivot; }
		void ComputeObjectBoundary() noexcept;

	public:

		virtual void Translate(const glm::vec3 &TranslationVector) ;
		virtual void RotateWorldSpace(const glm::vec3 &RotationAxisWorldSpace, float Degree) ;
		virtual void RotateLocalSpace(const glm::vec3 &RotationAxisLocalSpace, float Degree) ;
		virtual void RotateWithArbitraryAxis(const glm::vec3 &Position, const glm::vec3 &RotationAxis, float Degree);
		virtual void Scale(const glm::vec3& ScalingVector);
		virtual glm::mat4 GetModelMatrix() const;

		glm::vec3 GetWorldSpaceLocalOrigin() const;
		glm::vec3 GetWorldSpaceAxisX() const;
		glm::vec3 GetWorldSpaceAxisY() const;
		glm::vec3 GetWorldSpaceAxisZ() const;

	public:

		virtual void AllocateBuffers() noexcept;
		virtual void ReleaseBuffers() noexcept;

	public:

		bool CheckLoadingStatus() const noexcept;
		bool CheckLoaded() const noexcept { return isLoaded; }
		void SetLoaded() noexcept;
		void MarkPendingKill() { bPendingKill = true; }

		bool IsPendingKill() const { return bPendingKill; }
		CXCRect3D GetAABB() const noexcept { return AABB; };
		GLuint GetVAO() const { return m_VAO; }
		GLuint GetVertexCoordsVBO() const { return  m_VBO[0]; }
		GLuint GetTexCoordsVBO() const { return m_VBO[1]; }
		GLuint GetNormalsVBO() const { return m_VBO[2]; }
		
		std::shared_ptr<SubMesh> GetSubMesh(size_t Index);
		void AddSubMesh(std::shared_ptr<SubMesh> pNewMesh);

	protected:

		// Whether to release the mesh on next rendering tick
		bool bPendingKill;

		// Mesh Name
		std::string MeshName;

		// File name
		std::string FileName;
	
		// if obj file has been loaded
		bool isLoaded;

	protected:

		// Max, min and center coordinates
		glm::vec3 MaxCoords, MinCoords;

		// Pivot of rotation
		glm::vec3 Pivot;

		// AABB bounding box
		CXCRect3D AABB;

	protected:

		// Codes of the OctreeNode that contain the object
		std::unordered_set<std::string> m_OctreePtrs;
		
		// Child objects
		std::vector<std::weak_ptr<Mesh>> pChildNodes;

		// Parent object
		std::weak_ptr<Mesh> pParentNode;

		// Meshes 
		std::vector<std::shared_ptr<SubMesh>> SubMeshes;

	protected:

		// Model matrix
		glm::mat4 ModelMatrix;

		// Vertex index buffer
		std::vector<uint32_t> m_VertexIndices;

		// Vertex coordinate
		std::vector<glm::vec3> m_VertexCoords;

		// Vertex normals 
		std::vector<glm::vec3> m_VertexNormals;

		// UVs
		std::vector<glm::vec2> m_TexCoords;

		// ID of VBO, EBO and VAO
		GLuint  m_VBO[3], m_VAO;
	};

}
#endif // CXC_DRAWOBJECT_H

