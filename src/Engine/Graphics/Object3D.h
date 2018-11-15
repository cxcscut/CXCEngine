#ifdef WIN32

#include "..\General\DefineTypes.h"

#include"..\Libraries\TinyObjLoader\tiny_obj_loader.h"
#include "..\Physics\ode\ode.h"
#include "..\Physics\RigidBody3D.h"

#else

#include "../General/DefineTypes.h"

#include"../Libraries/TinyObjLoader/tiny_obj_loader.h"
#include "../Physics/ode/ode.h"
#include "../Physics/RigidBody3D.h"

#endif // WIN32

#ifndef CXC_DRAWOBJECT_H
#define CXC_DRAWOBJECT_H

#define CXC_NORMAL_FACTOR 0.2f
#define CXC_DIFFUSE_FACTOR (1.0f - CXC_NORMAL_FACTOR)

namespace cxc {

	class ShadowMapRender;
	class MaterialManager;
	class TextureManager;
	class Shape;
	class Material;

	class CXCRect3 {
		
	public:

		CXCRect3(const glm::vec3 &_max, const glm::vec3 &_min);
		CXCRect3();
		~CXCRect3();

		CXCRect3(const CXCRect3 &other);
		CXCRect3 &operator=(const CXCRect3 &other);
		
		bool isContain(const CXCRect3 &rhs) const noexcept;
		bool isIntersected(const CXCRect3 &other) const noexcept;

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

		// For customized type, overloaded operator < is needed for std::map
		bool operator<(const VertexIndexPacket &that) const {
			return memcmp((void*)this, (void*)&that, sizeof(VertexIndexPacket)) > 0;
		};
	};

	class Object3D : public RigidBody3D, public std::enable_shared_from_this<Object3D>
	{

	public:

		friend class SceneManager;
		friend class OctreeNode;
		friend class FBXSDKUtil;

		explicit Object3D();
		explicit Object3D(std::vector<glm::vec3>& Vertices,
			std::vector<glm::vec3>& Normals, 
			std::vector<glm::vec2>& UVs, 
			std::vector<uint32_t>& Indices);
		virtual ~Object3D();

		Object3D(const std::string &object_name);
		Object3D(const std::string &Object_name, const std::string &filename, const std::string &_tag = "" , GLboolean _enable = GL_TRUE);

	// Vertex Processing
	public:

		void ComputeNormal(glm::vec3 &normal,const glm::vec3 &vertex1,const glm::vec3 &vertex2,const glm::vec3 &vertex3) const noexcept;
		glm::vec3 GetPivot() const noexcept { return Pivot; }
		void SetPivot(const glm::vec3& NewPivot) noexcept { Pivot = NewPivot; }
		void ComputePivot() noexcept;
	// Model adjusting
	public:

		//virtual void CalculateSizeVector() noexcept;
		glm::vec3 CalculateRotatedCoordinate(const glm::vec3 &original_vec, const glm::vec3 &start,const glm::vec3 &direction,float degree) const noexcept;
		void ComputeObjectBoundary() noexcept;

	// Model transformation
	public:
		virtual void Translate(const glm::vec3 &TranslationVector) noexcept;
		virtual void RotateWorldSpace(const glm::vec3 &RotationAxis, float Degree) noexcept;
		virtual void RotateLocalSpace(const glm::vec3 &RotationAxis, float Degree) noexcept;

		// Rotation with arbitrary axis
		virtual void RotateWithArbitraryAxis(const glm::vec3 &start, const glm::vec3 &direction, float degree) noexcept;

		virtual void RenderingTick() noexcept;
		virtual void ShadowCastTick(ShadowMapRender* pRender) noexcept;

		void InitBuffers() noexcept;
		void ReleaseBuffers() noexcept;

	// Physics interface
	public:

		void InitializeRigidBody(dWorldID world,dSpaceID) noexcept;

		// 0 - gravity off
		// 1 - gravity on
		void SetObjectGravityMode(int mode) noexcept;

		void UpdateMeshTransMatrix() noexcept;

	// Private data access interface
	public:

		bool CheckLoadingStatus() const noexcept;
		void SetObjectName(const std::string &Name) noexcept;
		const std::string &GetObjectName() const noexcept;

		// Return scaling vector and centerizing vector
		bool CheckLoaded() const noexcept { return isLoaded; }
		void SetLoaded() noexcept;

		CXCRect3 GetAABB() const noexcept{return AABB; };

		GLboolean isEnable() const noexcept { return enable; };
		void Enable() noexcept { enable = GL_TRUE; };
		void Disable() noexcept { enable = GL_FALSE; };

		void SetTag(const std::string &_tag) noexcept { tag = _tag; };
		std::string CompareTag() noexcept { return tag; };

	private:

		// is enabled
		GLboolean enable;

		// File name
		std::string FileName;
		
		// Name
		std::string ObjectName;

		// Tag for collision detection
		std::string tag;
		
		// Max, min and center coordinates
		glm::vec3 MaxCoords, MinCoords;

		// Pivot of rotation
		glm::vec3 Pivot;

		// AABB bounding box
		CXCRect3 AABB;

		// <ShapeName , Material Name>
		std::unordered_map<std::string, std::string> Materials;

		// Codes of the OctreeNode that contain the object
		std::unordered_set<std::string> m_OctreePtrs;
		
		// Child objects
		std::vector<std::weak_ptr<Object3D>> pChildNodes;

		// Parent object
		std::weak_ptr<Object3D> pParentNode;

		// Model matrix
		glm::mat4 m_ModelMatrix;

		// Vertex index buffer
		std::vector<uint32_t> m_VertexIndices;

		// Vertex coordinate
		std::vector<glm::vec3> m_VertexCoords;

		// Vertex normals 
		std::vector<glm::vec3> m_VertexNormals;

		// UVs
		std::vector<glm::vec2> m_TexCoords;

		// Material
		std::shared_ptr<Material> pMaterial;

		// ID of VBO, EBO and VAO
		GLuint  m_VBO[3], m_EBO, m_VAO;

		// if obj file has been loaded
		bool isLoaded;

		// Kinematics object has infinite mass such as walls and earth.
		bool isKinematics;
	};

}
#endif // CXC_DRAWOBJECT_H

