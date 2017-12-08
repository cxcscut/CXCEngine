#include "..\General\DefineTypes.h"

#ifndef CXC_MODELLOADER_H
#define CXC_MODELLOADER_H

#include "..\Graphics\Object3D.h"
#include "..\Controller\RendererManager.h"
#include "..\Physics\RigidBody3D.h"

namespace cxc {

	class Shape : public RigidBody3D
	{

	public:

		friend class Object3D ;

		Shape();
		virtual ~Shape();

		explicit Shape(const Shape &) = delete;
		Shape& operator=(const Shape&) = delete;

		Shape(const Shape &&) = delete;
		Shape& operator=(const Shape&&) = delete;


	// Date access interface
	public:

		const std::vector<glm::vec3> &GetVertexArray() const noexcept;
		const std::vector<glm::vec3> &GetNormalArray() const noexcept;
		const std::vector<glm::vec3> &GetGeometricNormal() const noexcept;
		const std::vector<glm::vec2> &GetTexCoordArray() const noexcept;
		const std::vector<glm::vec3> &GetColorArray() const noexcept;
		const std::vector<uint32_t> &GetVertexIndices() const noexcept;
		std::string GetModelName() const noexcept;
		GLuint GetVertexNum() const noexcept;
		void SetObjectTreePtr(const std::shared_ptr<ObjectTree> &ptr) noexcept;
		void addKeyPoints(const glm::vec3 &point) noexcept;
		glm::vec3 LocalCoordsToGlobal(const glm::vec3 &Point) const noexcept;
		glm::vec3 GetCenterPos() const noexcept;
		glm::mat4 GetModelMatrix() const noexcept;
		std::shared_ptr<ObjectTree> GetMyPtr() const noexcept;
		std::vector<glm::vec3> &GetKeyPoints() noexcept;
		
		GLboolean CheckStateChanged() const noexcept;
		void SetStateChanged(GLboolean state) noexcept;

	// Vertex indexing computation
	public:

		GLboolean VertexisEqual(const glm::vec3 &lhs, const glm::vec3 &rhs) const noexcept;
		std::vector<glm::vec3> GetCurrentPos() noexcept;

	public:

		void CalculateReposMatrix(const glm::vec3 &scaling_vector,const glm::vec3 &centerize_vector) noexcept;
		void ComputeCenterPoint() noexcept;

	// Model transformation
	public:

		void Translate(const glm::vec3 &move_vector) noexcept;
		void Rotate(const glm::vec3 &rotation_axis, GLfloat degree) noexcept;

	private:

		// Name of the model
		std::string m_ModelName;

		// Model matrix
		glm::mat4 m_ModelMatrix;

		// Reposition matrix
		glm::mat4 m_ReposMatrix;

		// Transformation matrix recording transformation for each draw call
		glm::mat4 m_TransformationMatrix;

		// Vertex index buffer when using EBO
		std::vector<uint32_t> m_VertexIndices;

		// Vertex coordinate
		std::vector<glm::vec3> m_VertexCoords;

		// Vertex normals computed pre face
		std::vector<glm::vec3> m_VertexNormals;

		// Vertex color
		std::vector<glm::vec3> m_VertexColor;

		// UVs
		std::vector<glm::vec2> m_TexCoords;

		// Geometric normal pre vertex
		std::vector<glm::vec3> m_GeometricNormal;

		// Center position
		glm::vec3 m_CenterPos;

		// Material ID
		std::vector<GLuint> m_MaterialIDs;

		// Primitive type
		GLuint FragmentType;

		// Number of faces pre primitive
		GLuint Num_of_faces;

		// pointer to ObjectTree structure
		std::shared_ptr<ObjectTree> m_MyPtr;

		// key vertices
		std::vector<glm::vec3> m_KeyPoints;

		// ID of VBO, EBO and VAO
		GLuint  m_VBO[4], m_EBO, m_VAO;

		// state
		GLboolean stateChanged;
	};
}

#endif // CXC_MODELLOADER_H
