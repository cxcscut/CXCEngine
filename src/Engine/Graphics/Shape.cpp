
#include "Shape.h"

#ifdef WIN32

#include "..\EngineFacade\EngineFacade.h"
#include "..\Physics\RigidBody3D.h"

#else

#include "../EngineFacade/EngineFacade.h"
#include "../Physics/RigidBody3D.h"

#endif // WIN32

namespace cxc {

	Shape::Shape()
		: m_VertexIndices(),
		m_ModelMatrix(1.0f), stateChanged(GL_FALSE),
		 m_ModelName(""), FragmentType(3), Num_of_faces(0),
		m_VertexCoords(),m_VertexNormals(),m_TexCoords(),m_MaterialIDs(),m_VertexColor(),m_GeometricNormal(),
		m_MyPtr(nullptr),m_TransformationMatrix(1.0f),m_ReposMatrix(1.0f)
	{

	}

	Shape::~Shape()
	{
		m_VertexIndices.clear();
		m_VertexCoords.clear();
		m_VertexNormals.clear();
		m_TexCoords.clear();
	}

	GLboolean Shape::CheckStateChanged() const noexcept
	{
		return stateChanged;
	}

	void Shape::SetStateChanged(GLboolean state) noexcept
	{
		stateChanged = state;
	}

	std::vector<glm::vec3> &Shape::GetKeyPoints() noexcept
	{
		return m_KeyPoints;
	}

	const std::vector<glm::vec3> & Shape::GetVertexArray() const noexcept
	{
		return m_VertexCoords;
	}

	const std::vector<glm::vec3> & Shape::GetNormalArray() const noexcept
	{
		return m_VertexNormals;
	}

	const std::vector<glm::vec2> & Shape::GetTexCoordArray() const noexcept
	{
		return m_TexCoords;
	}

	const std::vector<uint32_t> &Shape::GetVertexIndices() const noexcept
	{
		return m_VertexIndices;
	}

	const std::vector<glm::vec3> &Shape::GetColorArray() const noexcept
	{
		return m_VertexColor;
	}

	void Shape::SetObjectTreePtr(const std::shared_ptr<ObjectTree> &ptr) noexcept
	{
		m_MyPtr = ptr;
	}

	glm::vec3 Shape::LocalCoordsToGlobal(const glm::vec3 &Point) const noexcept
	{
		glm::vec4 HomoPoint = glm::vec4(Point.x,Point.y,Point.z,1);

		glm::vec4 ret_HomoPoint = m_TransformationMatrix * HomoPoint;

		return glm::vec3(ret_HomoPoint.x, ret_HomoPoint.y, ret_HomoPoint.z);
	}

	void Shape::CalculateReposMatrix(const glm::vec3 &scaling_vector,const glm::vec3 &centerizing_vector) noexcept
	{
		m_ReposMatrix = glm::scale(glm::mat4(1.0f),scaling_vector);
		m_ReposMatrix = glm::translate(m_ReposMatrix, centerizing_vector);
	}

	const std::vector<glm::vec3> &Shape::GetGeometricNormal() const noexcept
	{
		return m_GeometricNormal;
	}

	GLuint Shape::GetVertexNum() const noexcept {
		return m_VertexCoords.size();
	}

	std::string Shape::GetModelName() const noexcept
	{
		return m_ModelName;
	}

	GLboolean Shape::VertexisEqual(const glm::vec3 &lhs, const glm::vec3 &rhs) const noexcept
	{
		glm::vec3 ret = lhs - rhs;
		return fabs(ret.x) < 0.001 && fabs(ret.y) < 0.001 && fabs(ret.z) < 0.001;
	}

	void Shape::UpdateTransMatrix() noexcept
	{
		m_TransformationMatrix = getTransMatrix();
	}

	void Shape::ComputeCenterPoint() noexcept
	{
		glm::vec3 center_point = glm::vec3(0.0f,0.0f,0.0f);

		for (auto it : m_VertexCoords)
			center_point += it;

		center_point /= m_VertexCoords.size();

		m_CenterPos = LocalCoordsToGlobal(center_point);
	}

	glm::vec3 Shape::GetCenterPos() const noexcept
	{
		return m_CenterPos;
	}

	std::shared_ptr<ObjectTree> Shape::GetMyPtr() const noexcept
	{
		return m_MyPtr;
	}

	void Shape::addKeyPoints(const glm::vec3 &point) noexcept
	{
		m_KeyPoints.emplace_back(point);
	}

	glm::mat4 Shape::GetModelMatrix() const noexcept
	{
		return m_ModelMatrix;
	}

	std::vector<glm::vec3> Shape::GetCurrentPos() noexcept
	{
		std::vector<glm::vec3> ret;
		ret.reserve(m_VertexCoords.size());

		for (auto &pos : m_VertexCoords)
		{
			auto homo_vec = glm::vec4({pos.x,pos.y,pos.z,1});
			auto new_vec = m_TransformationMatrix * homo_vec;
			ret.emplace_back(glm::vec3({ new_vec.x,new_vec.y,new_vec.z}));
		}

		return ret;
	}

	void Shape::Translate(const glm::vec3 &move_vector) noexcept
	{
		//auto TranlationMatrix = glm::translate(glm::mat4(1.0f),move_vector);

		// Left-multiplication with TransformationMatrix
		//m_TransformationMatrix = TranlationMatrix * m_TransformationMatrix;

		auto new_pos = getPosition() + move_vector;

		setPossition(new_pos.x,new_pos.y,new_pos.z);

		stateChanged = GL_TRUE;
	}

	void Shape::Rotate(const glm::vec3 &rotation_axis, GLfloat degree) noexcept
	{
		auto RotationMatrix = glm::rotate(glm::mat4(1.0f),degree, rotation_axis);

		// Left-multiplication with TransformationMatrix
		//m_TransformationMatrix = RotationMatrix * m_TransformationMatrix;

		setRotation(glm::mat3(RotationMatrix) * getRotation());

		stateChanged = GL_TRUE;
	}
}
