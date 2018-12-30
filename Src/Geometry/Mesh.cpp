#include "Geometry/Mesh.h"
#include "Geometry/SubMesh.h"
#include "Material/MaterialManager.h"
#include "Material/TextureManager.h"
#include "Utilities/FileHelper.h"
#include "World/World.h"
#include "Animation/AnimContext.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Image_loader/stb_image.h"

namespace cxc {

	Mesh::Mesh() :
		MeshName(""),
		isLoaded(false)
	{

		pParentNode.reset();

		auto max_float = std::numeric_limits<float>::max();

		// Initialize MinCoords with maximum float limit, MaxCoords with minimum float limit
		MinCoords = glm::vec3(max_float, max_float, max_float);
		MaxCoords = glm::vec3(-max_float, -max_float, -max_float);

	}

	Mesh::Mesh(const std::string& Name)
		: Mesh()
	{
		MeshName = Name;
	}

	Mesh::Mesh(std::vector<glm::vec3>& Vertices, std::vector<glm::vec3>& Normals):
		Mesh()
	{
		m_VertexCoords = Vertices;
		m_VertexNormals = Normals;

		// Compute the boundary of the object
		ComputeObjectBoundary();
	}

	Mesh::Mesh(std::vector<glm::vec3>& Vertices,
		std::vector<glm::vec3>& Normals,
		std::vector<glm::vec2>& UVs,
		std::vector<uint32_t>& Indices):
		Mesh()
	{
		m_VertexCoords = Vertices;
		m_VertexNormals = Normals;
		m_TexCoords = UVs;
		m_VertexIndices = Indices;

		// Compute the boundary of the object
		ComputeObjectBoundary();
	}


	Mesh::Mesh(const std::string &Name, const std::string &filename, const std::string &_tag, GLboolean _enable)
		: Mesh()
	{
		MeshName = Name;
		FileName = filename;
	}

	Mesh ::~Mesh()
	{
		pParentNode.reset();

		for (auto pChildNode : pChildNodes)
		{
			pChildNode.reset();
		}

		pChildNodes.clear();
	}

	void Mesh::SetLoaded() noexcept {
		isLoaded = true;
	}

	glm::vec3 Mesh::GetPivot() const noexcept
	{
		return (glm::vec3)(ModelMatrix * glm::vec4(Pivot, 1));
	}

	void Mesh::ComputeObjectBoundary() noexcept
	{
		for (auto Vertex : m_VertexCoords)
		{
			// Update MaxCoords
			MaxCoords.x = std::fmax(Vertex.x, MaxCoords.x);
			MaxCoords.y = std::fmax(Vertex.y, MaxCoords.y);
			MaxCoords.z = std::fmax(Vertex.z, MaxCoords.z);

			// Update MinCoords
			MinCoords.x = std::fmin(Vertex.x, MinCoords.x);
			MinCoords.y = std::fmin(Vertex.y, MinCoords.y);
			MinCoords.z = std::fmin(Vertex.z, MinCoords.z);
		}
	}

	void Mesh::AddSubMesh(std::shared_ptr<SubMesh> pNewMesh)
	{
		if (pNewMesh)
		{
			SubMeshes.push_back(pNewMesh);
			pNewMesh->SetOwnerObject(shared_from_this());
		}
	}

	std::shared_ptr<SubMesh> Mesh::GetSubMesh(size_t Index)
	{
		if (Index >= SubMeshes.size())
			return nullptr;
		else
			return SubMeshes[Index];
	}

	void Mesh::Scale(const glm::vec3& ScalingVector) 
	{
		auto ScalingMatrix = glm::scale(glm::mat4(1.0f), ScalingVector);
		ModelMatrix = ModelMatrix * ScalingMatrix;
	}

	glm::mat4 Mesh::GetModelMatrix() const
	{
		return ModelMatrix;
	}

	void Mesh::Translate(const glm::vec3 &TranslationVector) 
	{
		auto TranslationMatrix = glm::translate(glm::mat4(1.0f), TranslationVector);
		ModelMatrix = TranslationMatrix * ModelMatrix;
		
		// Perform translation on children node
		for (auto &pChildNode : pChildNodes)
		{
			if (!pChildNode.expired())
			{
				auto pNode = pChildNode.lock();
				pNode->Translate(TranslationVector);
			}
		}
	}

	void Mesh::RotateWorldSpace(const glm::vec3 &RotationAxisWorldSpace, float Degree) 
	{
		auto RotMatrix = glm::rotate(glm::mat4(1.0f), Degree, RotationAxisWorldSpace);
		ModelMatrix = RotMatrix * ModelMatrix;

		// Perform translation on children node
		for (auto &pChildNode : pChildNodes)
		{
			if (!pChildNode.expired())
			{
				auto pNode = pChildNode.lock();
				pNode->RotateWorldSpace(RotationAxisWorldSpace, Degree);
			}
		}
	}

	glm::vec3 Mesh::GetWorldSpaceLocalOrigin() const
	{
		return (glm::vec3)(GetModelMatrix() * glm::vec4(0, 0, 0, 1));
	}

	glm::vec3 Mesh::GetWorldSpaceAxisX() const
	{
		return glm::normalize((glm::vec3)(ModelMatrix * glm::vec4(1, 0, 0, 1)));
	}

	glm::vec3 Mesh::GetWorldSpaceAxisY() const
	{
		return glm::normalize((glm::vec3)(ModelMatrix * glm::vec4(0, 1, 0, 1)));
	}

	glm::vec3 Mesh::GetWorldSpaceAxisZ() const
	{
		return glm::normalize((glm::vec3)(ModelMatrix * glm::vec4(0, 0, 1, 1)));
	}

	void Mesh::RotateLocalSpace(const glm::vec3 &RotationAxisLocalSpace, float Degree) 
	{
		auto RotationAxisWorldSpace = (glm::vec3)(ModelMatrix * glm::vec4(RotationAxisLocalSpace, 1)) - GetWorldSpaceLocalOrigin();
		auto RotMatrix = glm::rotate(glm::mat4(1.0f), Degree, RotationAxisWorldSpace);
		ModelMatrix = ModelMatrix * RotMatrix;

		// Perform translation on children node
		for (auto &pChildNode : pChildNodes)
		{
			if (!pChildNode.expired())
			{
				auto pNode = pChildNode.lock();
				pNode->RotateLocalSpace(RotationAxisLocalSpace, Degree);
			}
		}
	}

	bool Mesh ::CheckLoadingStatus() const noexcept
	{
		return isLoaded;
	}

	void Mesh::AllocateBuffers() noexcept
	{
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		if (!m_VertexCoords.empty())
		{
			glGenBuffers(1, &m_VBO[0]);
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_VertexCoords.size(), &m_VertexCoords.front(), GL_STATIC_DRAW);
		}

		if (!m_TexCoords.empty())
		{
			glGenBuffers(1, &m_VBO[1]);
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * m_TexCoords.size(), &m_TexCoords.front(), GL_STATIC_DRAW);
		}

		if (!m_VertexNormals.empty())
		{
			glGenBuffers(1, &m_VBO[2]);
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO[2]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_VertexNormals.size(), &m_VertexNormals.front(), GL_STATIC_DRAW);
		}

		for (auto pSubMesh : SubMeshes)
		{
			if (pSubMesh)
			{
				pSubMesh->AllocateSubMeshEBO();
			}
		}
	}

	void Mesh::ReleaseBuffers() noexcept
	{
		if (glIsVertexArray(m_VAO))
		{
			glDeleteVertexArrays(1, &m_VAO);
		}

		for (auto pSubMesh : SubMeshes)
		{
			if (pSubMesh)
			{
				pSubMesh->ReleaseSubMeshEBO();
			}
		}

		if (glIsBuffer(m_VBO[0]))
		{
			glDeleteBuffers(1, &m_VBO[0]);
			glDisableVertexAttribArray(static_cast<GLuint>(Location::VERTEX_LOCATION));
		}

		if (glIsBuffer(m_VBO[1]))
		{
			glDeleteBuffers(1, &m_VBO[1]);
			glDisableVertexAttribArray(static_cast<GLuint>(Location::NORMAL_LOCATION));
		}

		if (glIsBuffer(m_VBO[2]))
		{
			glDeleteBuffers(1, &m_VBO[2]);
			glDisableVertexAttribArray(static_cast<GLuint>(Location::TEXTURE_LOCATION));
		}
	}

	void Mesh::RotateWithArbitraryAxis(const glm::vec3 &Position, const glm::vec3 &RotationAxis, float Degree) 
	{
		// In the global coordinate system, matrix multiplication sequence should be reversed
		// while in the local coordinate system, matrix multiplication sequence should not be reversed
		// here we use local coordinate system in which transformation matrix should be left-multiplied

		// Perform rotation on root node
		// Move to world origin
		glm::mat4 TransToOrigin = glm::translate(glm::mat4(1.0f), -Position);
		glm::mat4 RotMatrix = glm::rotate(glm::mat4(1.0f), Degree, RotationAxis);
		glm::mat4 TransBack = glm::translate(glm::mat4(1.0f), Position);

		ModelMatrix = TransBack * RotMatrix * TransToOrigin * ModelMatrix;

		// Perform rotation on children node
		for (auto pChildNode : pChildNodes)
		{
			if (!pChildNode.expired())
			{
				auto pNode = pChildNode.lock();
				pNode->RotateWithArbitraryAxis(Position, RotationAxis, Degree);
			}
		}
	}

	CXCRect3D::CXCRect3D(const glm::vec3 &_max, const glm::vec3 &_min) :max(_max), min(_min) {};

	CXCRect3D::CXCRect3D() :
		max(glm::vec3(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max())),
		min(glm::vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()))
	{}

	CXCRect3D::~CXCRect3D()
	{}

	CXCRect3D::CXCRect3D(const CXCRect3D &other)
	{
		max = other.max;
		min = other.min;
	}

	CXCRect3D &CXCRect3D::operator=(const CXCRect3D &other)
	{
		max = other.max;
		min = other.min;

		return *this;
	}

	bool CXCRect3D::isContain(const CXCRect3D &rhs) const noexcept
	{
		return rhs.max.x <= max.x && rhs.min.x >= min.x &&
			rhs.max.y <= max.y && rhs.min.y >= min.y &&
			rhs.max.z <= max.z && rhs.min.z >= min.z;
	}

	bool CXCRect3D::isIntersected(const CXCRect3D &other) const noexcept
	{
		return ((min.x >= other.min.x && min.x <= other.max.x) || (other.min.x >= min.x && other.min.x <= max.x)) &&
			((min.y >= other.min.y && min.y <= other.max.y) || (other.min.y >= min.y && other.min.y <= max.y)) &&
			((min.z >= other.min.z && min.z <= other.max.z) || (other.min.z >= min.z && other.min.z <= max.z));
	}
}

