#include "Shape.h"
#include "Material.h"
#include "Texture2D.h"

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
		 m_ModelName(""), FragmentType(3), Num_of_faces(0), pMaterial(nullptr),
		m_VertexCoords(),m_VertexNormals(),m_TexCoords(),m_MaterialIDs(),
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

	void Shape::ReleaseBuffers() noexcept
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

	void Shape::InitBuffers() noexcept
	{
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glGenBuffers(1, &m_VBO[0]);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_VertexCoords.size(), &m_VertexCoords.front(), GL_STATIC_DRAW);

		glGenBuffers(1, &m_VBO[1]);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) *m_VertexNormals.size(), &m_VertexNormals.front(), GL_STATIC_DRAW);

		glGenBuffers(1, &m_VBO[2]);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) *m_TexCoords.size(), &m_TexCoords.front(), GL_STATIC_DRAW);

		glGenBuffers(1, &m_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_VertexIndices.size(), &m_VertexIndices.front(), GL_STATIC_DRAW);
	}

	void Shape::RenderingTick() noexcept
	{
		auto pEngine = EngineFacade::GetInstance();
		auto pRender = pEngine->m_pSceneMgr->m_pRendererMgr->GetRenderPtr("StandardRender");
		if (!pRender) return;
		auto ProgramID = pRender->GetProgramID();

		auto pShadowRender = dynamic_cast<ShadowMapRender*>(pEngine->m_pSceneMgr->m_pRendererMgr->GetRenderPtr("ShadowRender"));
		if (!pShadowRender || pShadowRender->GetProgramID() <= 0)
			return;

		glUseProgram(ProgramID);
		glViewport(0, 0, pEngine->m_pWindowMgr->GetWindowWidth(), pEngine->m_pWindowMgr->GetWindowHeight());

		pEngine->m_pSceneMgr->BindCameraUniforms();
		pEngine->m_pSceneMgr->BindLightingUniforms(ProgramID);

		glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
		glm::vec3 eye_pos = pEngine->m_pSceneMgr->m_pCamera->eye_pos;

		GLuint TexSamplerHandle = glGetUniformLocation(ProgramID, "Sampler");
		GLuint texflag_loc = glGetUniformLocation(ProgramID, "isUseTex");
		GLuint depthBiasMVP_loc = glGetUniformLocation(ProgramID, "depthBiasMVP");
		GLuint ShadowMapSampler_loc = glGetUniformLocation(ProgramID, "shadowmap");
		GLuint Eyepos_loc = glGetUniformLocation(ProgramID, "EyePosition_worldspace");

		GLuint M_MatrixID = glGetUniformLocation(ProgramID, "M");
		GLuint Ka_loc = glGetUniformLocation(ProgramID, "Ka");
		GLuint Ks_loc = glGetUniformLocation(ProgramID, "Ks");
		GLuint Kd_loc = glGetUniformLocation(ProgramID, "Kd");
		GLuint isPointLight_loc = glGetUniformLocation(ProgramID, "isPointLight");
		GLuint shadowmapCube_loc = glGetUniformLocation(ProgramID, "shadowmapCube");

		glUniform1i(texflag_loc, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
		glEnableVertexAttribArray(static_cast<GLuint>(Location::VERTEX_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::VERTEX_LOCATION), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Vertex position

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[1]);
		glEnableVertexAttribArray(static_cast<GLuint>(Location::NORMAL_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::NORMAL_LOCATION), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Normal

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[2]);
		glEnableVertexAttribArray(static_cast<GLuint>(Location::TEXTURE_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::TEXTURE_LOCATION), 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Texcoords

		// Bind texture
		if (pMaterial && pMaterial->pTexture)
		{
			glActiveTexture(GL_TEXTURE0 + (GLuint)TextureUnit::UserTextureUnit);
			glBindTexture(GL_TEXTURE_2D, pMaterial->pTexture->GetTextureID());

			glUniform1i(TexSamplerHandle, 0);
			glUniform1i(texflag_loc, 1);

			glUniform3f(Ka_loc, pMaterial->AmbientFactor.x, pMaterial->AmbientFactor.y, pMaterial->AmbientFactor.z);
			glUniform3f(Kd_loc, pMaterial->DiffuseFactor.x, pMaterial->DiffuseFactor.y, pMaterial->DiffuseFactor.z);
			glUniform3f(Ks_loc, pMaterial->SpecularFactor.x, pMaterial->SpecularFactor.y, pMaterial->SpecularFactor.z);
		}

		// the bias for depthMVP to map the NDC coordinate from [-1,1] to [0,1] which is a necessity for texture sampling
		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);

		glm::mat4 depthBiasMVP;

		// Bind depth texture to the texture unit 1
		// We use texture unit 0 for the objectss texture sampling 
		// while texture unit 1 for depth buffer sampling
		glActiveTexture(GL_TEXTURE0 + (GLuint)TextureUnit::ShadowTextureUnit);
		if (pShadowRender->GetLightType() == eLightType::OmniDirectional)
			glBindTexture(GL_TEXTURE_CUBE_MAP, pShadowRender->GetShadowCubeMap());
		else
			glBindTexture(GL_TEXTURE_2D, pShadowRender->GetDepthTexture());

		// Draw the shape
		depthBiasMVP = biasMatrix * pShadowRender->GetDepthVP() * getTransMatrix();
		glUniformMatrix4fv(depthBiasMVP_loc, 1, GL_FALSE, &depthBiasMVP[0][0]);
		glUniform3f(Eyepos_loc, eye_pos.x, eye_pos.y, eye_pos.z);

		if (pShadowRender->GetLightType() == eLightType::OmniDirectional)
		{
			glUniform1i(shadowmapCube_loc, (GLuint)TextureUnit::ShadowTextureUnit);
			glUniform1i(isPointLight_loc, 1);
		}
		else
			glUniform1i(ShadowMapSampler_loc, (GLuint)TextureUnit::ShadowTextureUnit);

		glUniformMatrix4fv(M_MatrixID, 1, GL_FALSE, &getTransMatrix()[0][0]);

		// Note : the 4-th parameter of glDrawElements is the offset of EBO which must be sizeof(DataType) * number of indices
		glDrawElements(GL_TRIANGLES, m_VertexIndices.size(), GL_UNSIGNED_INT, &m_VertexIndices.front());
	}

	void Shape::ShadowCastTick(ShadowMapRender* pShadowRender) noexcept
	{
		auto pEngine = EngineFacade::GetInstance();

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

		glDrawElements(GL_TRIANGLES, m_VertexIndices.size(), GL_UNSIGNED_INT, &m_VertexIndices.front());
	}

	GLuint Shape::GetVertexNum() const noexcept 
	{
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
