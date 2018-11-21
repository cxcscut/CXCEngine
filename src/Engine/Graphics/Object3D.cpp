#include "Object3D.h"
#include "TextureManager.h"
#include "MaterialManager.h"

#ifdef WIN32

#include "..\Common\FileHelper.h"
#include "..\World\World.h"
#include "..\Graphics\RendererManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "..\Libraries\Image_loader\stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "..\Libraries\TinyObjLoader\tiny_obj_loader.h"

#else

#include "../Common/FileHelper.h"
#include "../Graphics/RendererManager.h"
#include "../World/World.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../Libraries/Image_loader/stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../Libraries/TinyObjLoader/tiny_obj_loader.h"

#endif // WIN32

namespace cxc {

	Object3D::Object3D() :
		ObjectName(""),
		isLoaded(false), enable(GL_TRUE), m_ModelMatrix(1.0f),
		isKinematics(false)
	{

		pParentNode.reset();

		auto max_float = std::numeric_limits<float>::max();

		// Initialize MinCoords with maximum float limit, MaxCoords with minimum float limit
		MinCoords = glm::vec3(max_float, max_float, max_float);
		MaxCoords = glm::vec3(-max_float, -max_float, -max_float);

	}

	Object3D::Object3D(std::vector<glm::vec3>& Vertices,
		std::vector<glm::vec3>& Normals,
		std::vector<glm::vec2>& UVs,
		std::vector<uint32_t>& Indices):
		Object3D()
	{
		m_VertexCoords = std::move(Vertices);
		m_VertexNormals = std::move(Normals);
		m_TexCoords = std::move(UVs);
		m_VertexIndices = std::move(Indices);

		// Compute the pivot after object being initialized
		ComputePivot();

		// Compute the boundary of the object
		ComputeObjectBoundary();
	}

	Object3D::Object3D(const std::string &sprite_name)
		: Object3D()
	{
		ObjectName = sprite_name;
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
	void Object3D::ComputePivot() noexcept
	{
		for (auto Vertex : m_VertexCoords)
		{
			Pivot += Vertex;
		}

		Pivot /= m_VertexCoords.size();
	}

	void Object3D::InitializeRigidBody(dWorldID world,dSpaceID space) noexcept
	{
		createRigidBody(world, reinterpret_cast<void*>(this));
		addCollider(space, m_VertexCoords, m_VertexIndices);

		if (isKinematics)
			dBodySetKinematic(GetBodyID());
	}

	const std::string &Object3D ::GetObjectName() const noexcept
	{
		return ObjectName;
	}

	void Object3D::Translate(const glm::vec3 &TranslationVector) noexcept
	{

		//auto TranlationMatrix = glm::translate(glm::mat4(1.0f),move_vector);

		// Left-multiplication with TransformationMatrix
		//m_TransformationMatrix = TranlationMatrix * m_TransformationMatrix;

		auto new_pos = getPosition() + TranslationVector;

		setPossition(new_pos.x, new_pos.y, new_pos.z);
		
		// Perform translation on children node
		for (auto &pChildNode : pChildNodes)
		{
			auto pNode = pChildNode.lock();
			pNode->Translate(TranslationVector);
		}

	}

	void Object3D::RotateWorldSpace(const glm::vec3 &RotationAxis, float Degree) noexcept
	{
		auto RotationMatrix = glm::rotate(glm::mat4(1.0f), Degree, RotationAxis);

		// Left-multiplication with TransformationMatrix
		//m_TransformationMatrix = RotationMatrix * m_TransformationMatrix;

		setRotation(glm::mat3(RotationMatrix) * getRotation());
		
		// Perform translation on children node
		for (auto &pChildNode : pChildNodes)
		{
			auto pNode = pChildNode.lock();
			pNode->RotateWorldSpace(RotationAxis, Degree);
		}

	}

	void Object3D::RotateLocalSpace(const glm::vec3 &RotationAxis, float Degree) noexcept
	{
		RotateWithArbitraryAxis(Pivot, RotationAxis, Degree);
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

		auto Forward = glm::translate(glm::mat4(1.0f),-start);

		auto RotMatrix = glm::rotate(glm::mat4(1.0f),degree,direction);

		auto Backward= glm::translate(glm::mat4(1.0f),start);

		auto TransMatrix = Backward * RotMatrix * Forward;

		auto homo_original_vec = glm::vec4(original_vec.x,original_vec.y,original_vec.z,1);

		auto new_vec = TransMatrix * homo_original_vec;

		return glm::vec3(new_vec.x / new_vec.w, new_vec.y / new_vec.w, new_vec.z / new_vec.w);
	}

	void Object3D::InitBuffers() noexcept
	{
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glGenBuffers(1, &m_VBO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_VertexCoords.size(), &m_VertexCoords.front(), GL_STATIC_DRAW);

		glGenBuffers(1, &m_VBO[1]);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * m_TexCoords.size(), &m_TexCoords.front(), GL_STATIC_DRAW);

		glGenBuffers(1, &m_VBO[2]);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_VertexNormals.size(), &m_VertexNormals.front(), GL_STATIC_DRAW);

		glGenBuffers(1, &m_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_VertexIndices.size(), &m_VertexIndices.front(), GL_STATIC_DRAW);
	}

	void Object3D::ReleaseBuffers() noexcept
	{
		if (m_VAO)
		{
			glDeleteVertexArrays(1, &m_VAO);
		}

		if (m_EBO)
		{
			glDeleteBuffers(1, &m_EBO);
		}

		if (m_VBO[0])
		{
			glDeleteBuffers(1, &m_VBO[0]);
			glDisableVertexAttribArray(static_cast<GLuint>(Location::VERTEX_LOCATION));
		}

		if (m_VBO[1])
		{
			glDeleteBuffers(1, &m_VBO[1]);
			glDisableVertexAttribArray(static_cast<GLuint>(Location::NORMAL_LOCATION));
		}

		if (m_VBO[2])
		{
			glDeleteBuffers(1, &m_VBO[2]);
			glDisableVertexAttribArray(static_cast<GLuint>(Location::TEXTURE_LOCATION));
		}
	}

	void Object3D::UpdateMeshTransMatrix() noexcept
	{
		UpdateMeshTransform();
	}
	
	void Object3D::SetObjectGravityMode(int mode) noexcept
	{
		setGravityMode(mode);
	}

	void Object3D::Tick(float DeltaSeconds) noexcept
	{
		GLint TexSamplerHandle, texflag_loc, depthBiasMVP_loc;
		GLint ShadowMapSampler_loc, Eyepos_loc, M_MatrixID;
		GLint Ka_loc, Ks_loc, Kd_loc, isPointLight_loc;
		GLint shadowmapCube_loc;

		auto pEngine = World::GetInstance();
		auto pRender = pEngine->m_pSceneMgr->m_pRendererMgr->GetRenderPtr("StandardRender");
		if (!pRender) return;
		auto ProgramID = pRender->GetProgramID();

		glUseProgram(ProgramID);
		glViewport(0, 0, pEngine->m_pWindowMgr->GetWindowWidth(), pEngine->m_pWindowMgr->GetWindowHeight());

		pEngine->m_pSceneMgr->BindCameraUniforms();
		pEngine->m_pSceneMgr->BindLightingUniforms(ProgramID);

		TexSamplerHandle = glGetUniformLocation(ProgramID, "Sampler");
		texflag_loc = glGetUniformLocation(ProgramID, "isUseTex");
		Eyepos_loc = glGetUniformLocation(ProgramID, "EyePosition_worldspace");
		M_MatrixID = glGetUniformLocation(ProgramID, "M");
		Ka_loc = glGetUniformLocation(ProgramID, "Ka");
		Ks_loc = glGetUniformLocation(ProgramID, "Ks");
		Kd_loc = glGetUniformLocation(ProgramID, "Kd");

		auto pShadowRender = dynamic_cast<ShadowMapRender*>(pEngine->m_pSceneMgr->m_pRendererMgr->GetRenderPtr("ShadowRender"));
		if (pShadowRender && pShadowRender->GetProgramID() > 0)
		{
			// Casting shadow
			isPointLight_loc = glGetUniformLocation(ProgramID, "isPointLight");
			shadowmapCube_loc = glGetUniformLocation(ProgramID, "shadowmapCube");
			depthBiasMVP_loc = glGetUniformLocation(ProgramID, "depthBiasMVP");
			ShadowMapSampler_loc = glGetUniformLocation(ProgramID, "shadowmap");

			// Bind depth texture to the texture unit 1
			// We use texture unit 0 for the objectss texture sampling 
			// while texture unit 1 for depth buffer sampling
			glActiveTexture(GL_TEXTURE0 + (GLuint)TextureUnit::ShadowTextureUnit);
			if (pShadowRender->GetLightType() == eLightType::OmniDirectional)
			{
				glBindTexture(GL_TEXTURE_CUBE_MAP, pShadowRender->GetShadowCubeMap());
				glUniform1i(shadowmapCube_loc, (GLuint)TextureUnit::ShadowTextureUnit);
				glUniform1i(isPointLight_loc, 1);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, pShadowRender->GetDepthTexture());
				glUniform1i(ShadowMapSampler_loc, (GLuint)TextureUnit::ShadowTextureUnit);
			}

			// the bias for depthMVP to map the NDC coordinate from [-1,1] to [0,1] which is a necessity for texture sampling
			glm::mat4 biasMatrix(
				0.5, 0.0, 0.0, 0.0,
				0.0, 0.5, 0.0, 0.0,
				0.0, 0.0, 0.5, 0.0,
				0.5, 0.5, 0.5, 1.0
			);

			glm::mat4 depthBiasMVP = biasMatrix * pShadowRender->GetDepthVP() * getTransMatrix();
			glUniformMatrix4fv(depthBiasMVP_loc, 1, GL_FALSE, &depthBiasMVP[0][0]);
		}

		glm::vec3 EyePosition = pEngine->m_pSceneMgr->m_pCamera->EyePosition;
		glUniform3f(Eyepos_loc, EyePosition.x, EyePosition.y, EyePosition.z);

		glUniform1i(texflag_loc, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
		glEnableVertexAttribArray(static_cast<GLuint>(Location::VERTEX_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::VERTEX_LOCATION), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Vertex position

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
		glEnableVertexAttribArray(static_cast<GLuint>(Location::TEXTURE_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::TEXTURE_LOCATION), 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Texcoords

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[2]);
		glEnableVertexAttribArray(static_cast<GLuint>(Location::NORMAL_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::NORMAL_LOCATION), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Normal

		// Set model matrix
		glUniformMatrix4fv(M_MatrixID, 1, GL_FALSE, &getTransMatrix()[0][0]);

		for (auto pMesh : Meshes)
		{
			// Bind the material of the mesh
			pMesh->BindMaterial(Ka_loc, Kd_loc, Ks_loc, TexSamplerHandle);

			// Draw the mesh
			pMesh->DrawMesh();
		}
	}

	void Object3D::CastingShadows(ShadowMapRender* pShadowRender) noexcept
	{
		auto pEngine = World::GetInstance();

		glViewport(0, 0, pShadowRender->GetWidth(), pShadowRender->GetHeight());

		glUseProgram(pShadowRender->GetProgramID());

		GLuint depthMVP_Loc = glGetUniformLocation(pShadowRender->GetProgramID(), "depthMVP");

		glm::mat4 depthMVP;

		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
		glEnableVertexAttribArray(static_cast<GLuint>(Location::VERTEX_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::VERTEX_LOCATION), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Vertex position

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

		// Render depth map of the shape
		depthMVP = pShadowRender->GetDepthVP() * getTransMatrix();

		glUniformMatrix4fv(depthMVP_Loc, 1, GL_FALSE, &depthMVP[0][0]);

		glDrawElements(GL_TRIANGLES, m_VertexIndices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	}

	void Object3D::RotateWithArbitraryAxis(const glm::vec3 &start, const glm::vec3 &direction, float degree) noexcept
	{
		// In the global coordinate system, matrix multiplication sequence should be reversed
		// while in the local coordinate system, matrix multiplication sequence should not be reversed
		// here we use local coordinate system in which transformation matrix should be left-multiplied

		// Perform rotation on root node
		// Move to world origin
		glm::mat4 M = glm::translate(glm::rotate(glm::translate(glm::mat4(1.0f),start),degree,direction),-start);

		// Perform rotation on root node
		// Set 3x3 Rotation matrix
		setRotation((glm::mat3)M * getRotation());

		auto P = getPosition();

		// Set position vector
		setPossition(
			M[0][0] * P.x + M[1][0] * P.y + M[2][0] * P.z + M[3][0],
			M[0][1] * P.x + M[1][1] * P.y + M[2][1] * P.z + M[3][1],
			M[0][2] * P.x + M[1][2] * P.y + M[2][2] * P.z + M[3][2]
		);

		// Perform rotation on children node
		for (auto pChildNode : pChildNodes)
		{
			auto pNode = pChildNode.lock();
			pNode->RotateWithArbitraryAxis(start, direction, degree);
		}
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

