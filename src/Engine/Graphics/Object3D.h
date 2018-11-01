#ifdef WIN32

#include "..\General\DefineTypes.h"

#include"..\Libraries\TinyObjLoader\tiny_obj_loader.h"
#include "..\Physics\ode\ode.h"

#else

#include "../General/DefineTypes.h"

#include"../Libraries/TinyObjLoader/tiny_obj_loader.h"
#include "../Physics/ode/ode.h"

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

	class Object3D : public std::enable_shared_from_this<Object3D>
	{

	public:

		friend class SceneManager;
		friend class OctreeNode;

		explicit Object3D();
		virtual ~Object3D();

		Object3D(const std::string &object_name);
		Object3D(const std::string &Object_name, const std::string &filename, const std::string &_tag = "" , GLboolean _enable = GL_TRUE);

	// Obj file loading
	public:

		GLboolean LoadOBJFromFile(const std::string &filename);
		GLboolean LoadingObjectModel() { return LoadOBJFromFile(FileName); };

	// Vertex Processing
	public:

		void ComputeNormal(glm::vec3 &normal,const glm::vec3 &vertex1,const glm::vec3 &vertex2,const glm::vec3 &vertex3) const noexcept;
		void IndexVertex(std::map<VertexIndexPacket, uint32_t> &VertexIndexMap,const glm::vec3 &geo_normal,const VertexIndexPacket &vertex,
			uint32_t &subindex, Shape* drawobject,const tinyobj::attrib_t &attrib);

	// Model adjusting
	public:

		//virtual void CalculateSizeVector() noexcept;
		glm::vec3 CalculateRotatedCoordinate(const glm::vec3 &original_vec, const glm::vec3 &start,const glm::vec3 &direction,float degree) const noexcept;
		void UpdateBoundaryCoords(const glm::vec3 &pos) noexcept;

	// Assigning
	public:

		bool AddRoot(const std::string &ModelName) noexcept;
		bool AddChild(const std::string &RootModelName,const std::string &ChildModelName) noexcept;
		bool FindChild(const std::string &RootModelName,const std::string &TargetModelName,std::shared_ptr<ObjectTree> &RetModelPtr) noexcept;

	// Model transformation
	public:
		virtual void Translation(const std::string &ModelName, const glm::vec3 &TranslationVector) noexcept;
		virtual void Translation(const glm::vec3 &TranslationVector) noexcept;

		virtual void Rotation(const std::string &ModelName, const glm::vec3 &RotationAxis, float Degree) noexcept;
		virtual void Rotation(const glm::vec3 &RotationAxis, float Degree) noexcept;

		// Rotation with arbitrary axis
		virtual void RotateWithArbitraryAxis(const std::string &ModelName, const glm::vec3 &start, const glm::vec3 &direction, float degree) noexcept;

		virtual void RenderingTick() noexcept;
		virtual void DrawShadow(ShadowMapRender* pRender) noexcept;

		void InitBuffers() noexcept;
		void ReleaseBuffers() noexcept;

	// Physics interface
	public:

		void InitializeRigidBodies(dWorldID world,dSpaceID) noexcept;

		// 0 - gravity off
		// 1 - gravity on
		void SetObjectGravityMode(int mode) noexcept;

		void UpdateMeshTransMatrix() noexcept;

	// Private data access interface
	public:

		bool CheckLoadingStatus() const noexcept;
		void SetObjectName(const std::string &Name) noexcept;
		bool CheckCompoent(const std::string &CompoentName) const noexcept;
		const std::string &GetObjectName() const noexcept;

		// Return Model ptr
		std::shared_ptr<Shape> GetModelByName(const std::string &ModelName) const noexcept;

		// Return Model names
		std::vector<std::string> GetModelNames() const noexcept;

		// Return scaling vector and centerizing vector
		std::unordered_map<std::string, std::shared_ptr<Shape>> &GetModelMap() noexcept;
		std::vector<std::shared_ptr<ObjectTree>> &GetObjectTreePtr() noexcept;
		bool CheckLoaded() const noexcept { return isLoaded; }
		void SetLoaded() noexcept;

		uint32_t GetObjectVertexNum() const noexcept;
		CXCRect3 GetAABB() const noexcept{return AABB; };

		uint32_t GetVertexSubscript(const std::string &shape_name) noexcept;

		void ComputeCenterPos() noexcept;
		glm::vec3 GetCenterPos() const noexcept { return CenterCoords; };

		GLboolean CheckStateChanged() const noexcept;
		void SetStateChanged(GLboolean state) noexcept;

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
		glm::vec3 MaxCoords, MinCoords, CenterCoords;

		// AABB bounding box
		CXCRect3 AABB;

		// <ShapeName , Pointer to Model>
		std::unordered_map<std::string,std::shared_ptr<Shape>> m_ModelMap;

		// <ShapeName , Material Name>
		std::unordered_map<std::string, std::string> Materials;

		// Subscript of shape's vertex in vbo
		std::unordered_map<std::string, uint32_t> m_VertexSubscript;

		// Object tree
		std::vector<std::shared_ptr<ObjectTree>> m_ObjectTree;

		// Codes of the OctreeNode that contain the object
		std::unordered_set<std::string> m_OctreePtrs;

		GLboolean stateChanged;

		// if obj file has been loaded
		bool isLoaded;

		// Kinematics object has infinite mass such as walls and earth.
		bool isKinematics;
	};

}
#endif // CXC_DRAWOBJECT_H

