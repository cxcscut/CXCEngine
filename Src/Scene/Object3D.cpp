#include "Scene/Object3D.h"
#include "Material/MaterialManager.h"
#include "Material/TextureManager.h"
#include "Utilities/FileHelper.h"
#include "World/World.h"
#include "Animation/AnimContext.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Image_loader/stb_image.h"

namespace cxc {

	Object3D::Object3D() :
		ObjectName(""),
		isLoaded(false), enable(GL_TRUE)
	{

		pParentNode.reset();

		auto max_float = std::numeric_limits<float>::max();

		// Initialize MinCoords with maximum float limit, MaxCoords with minimum float limit
		MinCoords = glm::vec3(max_float, max_float, max_float);
		MaxCoords = glm::vec3(-max_float, -max_float, -max_float);

	}

	Object3D::Object3D(std::vector<glm::vec3>& Vertices, std::vector<glm::vec3>& Normals):
		Object3D()
	{
		m_VertexCoords = Vertices;
		m_VertexNormals = Normals;

		// Compute the boundary of the object
		ComputeObjectBoundary();
	}

	Object3D::Object3D(std::vector<glm::vec3>& Vertices,
		std::vector<glm::vec3>& Normals,
		std::vector<glm::vec2>& UVs,
		std::vector<uint32_t>& Indices):
		Object3D()
	{
		m_VertexCoords = Vertices;
		m_VertexNormals = Normals;
		m_TexCoords = UVs;
		m_VertexIndices = Indices;

		// Compute the boundary of the object
		ComputeObjectBoundary();
	}

	Object3D::Object3D(const std::string &object_name)
		: Object3D()
	{
		ObjectName = object_name;
	}

	Object3D::Object3D(const std::string &Object_name, const std::string &filename, const std::string &_tag, GLboolean _enable)
		: Object3D()
	{
		ObjectName = Object_name;
		FileName = filename;
		tag = _tag;
		enable = _enable;
	}

	Object3D ::~Object3D()
	{
		pParentNode.reset();

		for (auto pChildNode : pChildNodes)
		{
			pChildNode.reset();
		}

		pChildNodes.clear();
	}

	void Object3D::SetLoaded() noexcept {
		isLoaded = true;
	}

	glm::vec3 Object3D::GetPivot() const noexcept
	{
		return (glm::vec3)(ModelMatrix * glm::vec4(Pivot, 1));
	}

	void Object3D::ComputeNormal(glm::vec3 &normal, const glm::vec3 &vertex1, const glm::vec3 &vertex2, const glm::vec3 &vertex3) const noexcept
	{
		glm::vec3 vector12 = vertex2 - vertex1;
		glm::vec3 vector13 = vertex3 - vertex1;

		normal = glm::normalize(glm::cross(vector12,vector13));
	}

	void Object3D::ComputeObjectBoundary() noexcept
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

	void Object3D::AddMesh(std::shared_ptr<Mesh> pNewMesh)
	{
		if (pNewMesh)
		{
			Meshes.push_back(pNewMesh);
			pNewMesh->SetOwnerObject(shared_from_this());
		}
	}

	std::shared_ptr<Mesh> Object3D::GetMesh(uint16_t Index)
	{
		if (Index >= Meshes.size())
			return nullptr;
		else
			return Meshes[Index];
	}

	const std::string &Object3D ::GetObjectName() const noexcept
	{
		return ObjectName;
	}

	void Object3D::Scale(const glm::vec3& ScalingVector) 
	{
		auto ScalingMatrix = glm::scale(glm::mat4(1.0f), ScalingVector);
		ModelMatrix = ModelMatrix * ScalingMatrix;
	}

	glm::mat4 Object3D::GetModelMatrix() const
	{
		return ModelMatrix;
	}

	void Object3D::Translate(const glm::vec3 &TranslationVector) 
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

	void Object3D::RotateWorldSpace(const glm::vec3 &RotationAxisWorldSpace, float Degree) 
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

	glm::vec3 Object3D::GetWorldSpaceLocalOrigin() const
	{
		return (glm::vec3)(GetModelMatrix() * glm::vec4(0, 0, 0, 1));
	}

	glm::vec3 Object3D::GetWorldSpaceAxisX() const
	{
		return glm::normalize((glm::vec3)(ModelMatrix * glm::vec4(1, 0, 0, 1)));
	}

	glm::vec3 Object3D::GetWorldSpaceAxisY() const
	{
		return glm::normalize((glm::vec3)(ModelMatrix * glm::vec4(0, 1, 0, 1)));
	}

	glm::vec3 Object3D::GetWorldSpaceAxisZ() const
	{
		return glm::normalize((glm::vec3)(ModelMatrix * glm::vec4(0, 0, 1, 1)));
	}

	void Object3D::RotateLocalSpace(const glm::vec3 &RotationAxisLocalSpace, float Degree) 
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

	void Object3D ::SetObjectName(const std::string &name) noexcept
	{
		ObjectName = name;
	}

	bool Object3D ::CheckLoadingStatus() const noexcept
	{
		return isLoaded;
	}

	glm::vec3 Object3D ::CalculateRotatedCoordinate(const glm::vec3 &original_vec, const glm::vec3 &start, const glm::vec3 &direction,float degree) const noexcept
	{

		auto Forward = glm::translate(glm::mat4(1.0f), -start);

		auto RotMatrix = glm::rotate(glm::mat4(1.0f), degree, direction);

		auto Backward= glm::translate(glm::mat4(1.0f), start);

		auto TransMatrix = Backward * RotMatrix * Forward;

		auto homo_original_vec = glm::vec4(original_vec.x, original_vec.y, original_vec.z, 1);

		auto new_vec = TransMatrix * homo_original_vec;

		return glm::vec3(new_vec.x / new_vec.w, new_vec.y / new_vec.w, new_vec.z / new_vec.w);
	}

	void Object3D::InitBuffers() noexcept
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

		if (!m_VertexCoords.empty())
		{
			glGenBuffers(1, &m_VBO[2]);
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO[2]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_VertexNormals.size(), &m_VertexNormals.front(), GL_STATIC_DRAW);
		}

		for (auto pMesh : Meshes)
		{
			if (pMesh)
			{
				pMesh->AllocateMeshEBO();
			}
		}
	}

	void Object3D::ReleaseBuffers() noexcept
	{
		if (glIsVertexArray(m_VAO))
		{
			glDeleteVertexArrays(1, &m_VAO);
		}

		for (auto pMesh : Meshes)
		{
			if (pMesh)
			{
				pMesh->ReleaseMeshEBO();
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

	void Object3D::Tick(float DeltaSeconds)
	{
		// Animating 
		if (pAnimContext)
		{
			pAnimContext->Tick(DeltaSeconds);
		}
	}

	void Object3D::PreRender(const std::vector<std::shared_ptr<LightSource>>& Lights) noexcept
	{
		for (auto pMesh : Meshes)
		{
			auto pRenderMgr = RendererManager::GetInstance();
			pRenderMgr->SetCurrentUsedRender(pMesh->GetMeshRender());
			auto pMeshRender = pMesh->GetMeshRender();
			if (pMeshRender)
			{
				pMeshRender->PreRender(pMesh, Lights);
			}
		}
	}

	void Object3D::Render(const std::vector<std::shared_ptr<LightSource>>& Lights) noexcept
	{
		for (auto pMesh : Meshes)
		{
			auto pRenderMgr = RendererManager::GetInstance();
			pRenderMgr->SetCurrentUsedRender(pMesh->GetMeshRender());
			auto pMeshRender = pMesh->GetMeshRender();
			if (pMeshRender)
			{
				pMeshRender->Render(pMesh, Lights);
			}
		}
	}

	void Object3D::PostRender(const std::vector<std::shared_ptr<LightSource>>& Lights) noexcept
	{
		for (auto pMesh : Meshes)
		{
			auto pRenderMgr = RendererManager::GetInstance();
			pRenderMgr->SetCurrentUsedRender(pMesh->GetMeshRender());
			auto pMeshRender = pMesh->GetMeshRender();
			if (pMeshRender)
			{
				pMeshRender->PostRender(pMesh, Lights);
			}
		}
	}

	void Object3D::RotateWithArbitraryAxis(const glm::vec3 &Position, const glm::vec3 &RotationAxis, float Degree) 
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

	void Object3D::CreateAnimationContext()
	{
		pAnimContext = NewObject<AnimContext>(shared_from_this());
	}

	CXCRect3::CXCRect3(const glm::vec3 &_max, const glm::vec3 &_min) :max(_max), min(_min) {};

	CXCRect3::CXCRect3() :
		max(glm::vec3(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max())),
		min(glm::vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()))
	{}

	CXCRect3::~CXCRect3()
	{}

	CXCRect3::CXCRect3(const CXCRect3 &other)
	{
		max = other.max;
		min = other.min;
	}

	CXCRect3 &CXCRect3::operator=(const CXCRect3 &other)
	{
		max = other.max;
		min = other.min;

		return *this;
	}

	bool CXCRect3::isContain(const CXCRect3 &rhs) const noexcept
	{
		return rhs.max.x <= max.x && rhs.min.x >= min.x &&
			rhs.max.y <= max.y && rhs.min.y >= min.y &&
			rhs.max.z <= max.z && rhs.min.z >= min.z;
	}

	bool CXCRect3::isIntersected(const CXCRect3 &other) const noexcept
	{
		return ((min.x >= other.min.x && min.x <= other.max.x) || (other.min.x >= min.x && other.min.x <= max.x)) &&
			((min.y >= other.min.y && min.y <= other.max.y) || (other.min.y >= min.y && other.min.y <= max.y)) &&
			((min.z >= other.min.z && min.z <= other.max.z) || (other.min.z >= min.z && other.min.z <= max.z));
	}
}

