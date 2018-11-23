#include "RenderPipeline.h"

#ifdef WIN32

#include "..\Graphics\Object3D.h"
#include "..\World\World.h"
#include "..\Graphics\SceneManager.h"
#include "..\Graphics\Lighting.h"

#else

#include "../Graphics/Object3D.h"
#include "../Graphics/SceneManager.h"
#include "../World/World.h"
#include "../Graphics/Lighting.h"

#endif

namespace cxc
{
	RenderPipeline::RenderPipeline() :
		VertexShaderID(-1), FragmentShaderID(-1),
		ProgramID(-1)
	{
		pOwnerRender.reset();
	}

	RenderPipeline::RenderPipeline(const std::string& Name)
		: RenderPipeline()
	{
		PipelineName = Name;
	}

	RenderPipeline::~RenderPipeline()
	{
		if (VertexShaderID)
		{
			if(ProgramID)
				glDetachShader(ProgramID, VertexShaderID);

			glDeleteShader(VertexShaderID);
		}

		if (FragmentShaderID)
		{
			if (ProgramID)
				glDetachShader(ProgramID, FragmentShaderID);

			glDeleteShader(FragmentShaderID);
		}

		if (ProgramID)
			glDeleteProgram(ProgramID);

		pOwnerRender.reset();
	}

	void RenderPipeline::UsePipeline()
	{
		glUseProgram(ProgramID);
	}

	void RenderPipeline::BindLightUniforms(std::shared_ptr<BaseLighting> pLight)
	{
		GLuint LightID = glGetUniformLocation(ProgramID, "LightPosition_worldspace");
		auto LightPos = pLight->GetLightPos();
		glUniform3f(LightID, LightPos.x, LightPos.y, LightPos.z);
	}

	bool RenderPipeline::InitializePipeline()
	{
		VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		ProgramID = glCreateProgram();

		std::string VertexShaderSourceCode, FragmentShaderSourceCode;
		std::string OutResultLog;

		// Get source code of the vertex shader
		bool bSuccessful = true;
		bSuccessful &= ReadShaderSourceCode(VertexShaderPath, VertexShaderSourceCode);
		if (!bSuccessful)
		{
			std::cout << "VS load failed, Path : " << VertexShaderPath << std::endl;
			return false;
		}

		// Get source code of the fragment shader
		bSuccessful &= ReadShaderSourceCode(FragmentShaderPath, FragmentShaderSourceCode);
		if (!bSuccessful)
		{
			std::cout << "FS load failed, Path : " << FragmentShaderPath << std::endl;
			return false;
		}

		// Compile the vertex shader
		bSuccessful &= CompileShader(VertexShaderID, VertexShaderSourceCode, OutResultLog);
		if (!bSuccessful)
		{
			std::cout << "VS compile failed, Path : " << VertexShaderPath << std::endl;
			std::cout << OutResultLog << std::endl;
			return false;
		}

		// Compile the fragment shader
		bSuccessful &= CompileShader(FragmentShaderID, FragmentShaderSourceCode, OutResultLog);
		if (!bSuccessful)
		{
			std::cout << "FS compile failed, Path : " << FragmentShaderPath << std::endl;
			std::cout << OutResultLog << std::endl;
			return false;
		}

		// Link the shaders
		LinkShaders();
		bSuccessful &= CheckLinkingStatus(ProgramID);
		if (!bSuccessful)
		{
			std::cout << "Program linking failed" << FragmentShaderPath << std::endl;
			return false;
		}

		return true;
	}

	bool RenderPipeline::ReadShaderSourceCode(const std::string& ShaderPath, std::string& SourceCode)
	{
		if (!FileHelper::FileIsExists(ShaderPath))
			return false;

		SourceCode.clear();
		std::ifstream ShaderStream(ShaderPath, std::ios::in);
		if (ShaderStream.is_open())
		{
			std::string Line = "";
			while (getline(ShaderStream, Line))
			{
				SourceCode += "\n" + Line;
			}
			ShaderStream.close();
		}
		else
			return false;

		return true;
	}

	bool RenderPipeline::CompileShader(GLuint ShaderID, const std::string& SourceCode, std::string& OutResultLog)
	{
		const char* SourceCodeStr = SourceCode.c_str();
		glShaderSource(ShaderID, 1, &SourceCodeStr, nullptr);
		glCompileShader(ShaderID);

		if (CheckCompilingStatus(ShaderID) != true)
		{
			char szLog[1024] = {0};
			GLsizei Loglen = 0;
			glGetShaderInfoLog(ShaderID, 1024, &Loglen, szLog);
			OutResultLog = szLog;

			return false;
		}
		else
			return true;
	}

	bool RenderPipeline::CheckCompilingStatus(GLuint ShaderID) const
	{
		GLint Result;
		glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);

		return Result > 0 ? true : false;
	}

	void RenderPipeline::PreRender(std::shared_ptr<Mesh> pMesh)
	{
		// Do noting in the base render class
	}

	void RenderPipeline::Render(std::shared_ptr<Mesh> pMesh)
	{
		GLint TexSamplerHandle, texflag_loc;
		GLint Eyepos_loc, M_MatrixID;
		GLint Ka_loc, Ks_loc, Kd_loc;

		auto pWorld = World::GetInstance();
		auto pRender = pOwnerRender.lock();
		auto pOwnerObject = pMesh->GetOwnerObject();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, pWorld->pWindowMgr->GetWindowWidth(), pWorld->pWindowMgr->GetWindowHeight());
		BindLightUniforms(pRender->GetLightInfo());

		TexSamplerHandle = glGetUniformLocation(ProgramID, "Sampler");
		texflag_loc = glGetUniformLocation(ProgramID, "isUseTex");
		Eyepos_loc = glGetUniformLocation(ProgramID, "EyePosition_worldspace");
		M_MatrixID = glGetUniformLocation(ProgramID, "M");
		Ka_loc = glGetUniformLocation(ProgramID, "Ka");
		Ks_loc = glGetUniformLocation(ProgramID, "Ks");
		Kd_loc = glGetUniformLocation(ProgramID, "Kd");

		glm::vec3 EyePosition = pWorld->pSceneMgr->pCamera->EyePosition;
		glUniform3f(Eyepos_loc, EyePosition.x, EyePosition.y, EyePosition.z);

		glUniform1i(texflag_loc, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glBindVertexArray(pOwnerObject->GetVAO());

		glBindBuffer(GL_ARRAY_BUFFER, pOwnerObject->GetVertexCoordsVBO());
		glEnableVertexAttribArray(static_cast<GLuint>(Location::VERTEX_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::VERTEX_LOCATION), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Vertex position

		glBindBuffer(GL_ARRAY_BUFFER, pOwnerObject->GetTexCoordsVBO());
		glEnableVertexAttribArray(static_cast<GLuint>(Location::TEXTURE_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::TEXTURE_LOCATION), 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Texcoords

		glBindBuffer(GL_ARRAY_BUFFER, pOwnerObject->GetNormalsVBO());
		glEnableVertexAttribArray(static_cast<GLuint>(Location::NORMAL_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::NORMAL_LOCATION), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Normal

		// Set model matrix																										  
		glUniformMatrix4fv(M_MatrixID, 1, GL_FALSE, &pOwnerObject->getTransMatrix()[0][0]);

		// Bind the material of the mesh
		pMesh->BindMaterial(Ka_loc, Kd_loc, Ks_loc, TexSamplerHandle);

		// Draw the mesh
		pMesh->DrawMesh();
	}

	void RenderPipeline::PostRender(std::shared_ptr<Mesh> pMesh)
	{
		// Do noting in the base render class
	}

	bool RenderPipeline::CheckLinkingStatus(GLuint ProgramID) const
	{
		GLint Result;
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);

		return Result > 0 ? true : false;
	}

	void RenderPipeline::LinkShaders()
	{
		// Attach vertex shader
		glAttachShader(ProgramID, VertexShaderID);

		// Attach fragment shader
		glAttachShader(ProgramID, FragmentShaderID);

		// Link program
		glLinkProgram(ProgramID);
	}

	ShadowedMeshRenderPipeline::ShadowedMeshRenderPipeline() :
		RenderPipeline("ShadowedMeshRenderPipeline")
	{

	}

	ShadowedMeshRenderPipeline::~ShadowedMeshRenderPipeline()
	{

	}

	void ShadowedMeshRenderPipeline::PreRender(std::shared_ptr<Mesh> pMesh)
	{

	}

	void ShadowedMeshRenderPipeline::Render(std::shared_ptr<Mesh> pMesh)
	{
		GLint TexSamplerHandle, texflag_loc, depthBiasMVP_loc;
		GLint ShadowMapSampler_loc, Eyepos_loc, M_MatrixID;
		GLint Ka_loc, Ks_loc, Kd_loc, isPointLight_loc;
		GLint shadowmapCube_loc;

		auto pWorld = World::GetInstance();
		auto pRender = pOwnerRender.lock();
		auto pShadowRender = dynamic_cast<ShadowMapRender*>(pRender.get());
		if (!pShadowRender)
			return;

		auto pOwnerObject = pMesh->GetOwnerObject();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, pWorld->pWindowMgr->GetWindowWidth(), pWorld->pWindowMgr->GetWindowHeight());
		BindLightUniforms(pRender->GetLightInfo());

		TexSamplerHandle = glGetUniformLocation(ProgramID, "Sampler");
		texflag_loc = glGetUniformLocation(ProgramID, "isUseTex");
		Eyepos_loc = glGetUniformLocation(ProgramID, "EyePosition_worldspace");
		M_MatrixID = glGetUniformLocation(ProgramID, "M");
		Ka_loc = glGetUniformLocation(ProgramID, "Ka");
		Ks_loc = glGetUniformLocation(ProgramID, "Ks");
		Kd_loc = glGetUniformLocation(ProgramID, "Kd");

		// Casting shadow
		isPointLight_loc = glGetUniformLocation(ProgramID, "isPointLight");
		shadowmapCube_loc = glGetUniformLocation(ProgramID, "shadowmapCube");
		depthBiasMVP_loc = glGetUniformLocation(ProgramID, "depthBiasMVP");
		ShadowMapSampler_loc = glGetUniformLocation(ProgramID, "shadowmap");

		// Bind depth texture to the texture unit 1
		// We use texture unit 0 for the objectss texture sampling 
		// while texture unit 1 for depth buffer sampling
		glActiveTexture(GL_TEXTURE0 + (GLuint)TextureUnit::ShadowTextureUnit);
		if (pRender->GetLightInfo()->GetLightType() == eLightType::OmniDirectional)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, pShadowRender->GetShadowCubeMap());
			glUniform1i(shadowmapCube_loc, (GLuint)TextureUnit::ShadowTextureUnit);
			glUniform1i(isPointLight_loc, 1);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, pShadowRender->GetShadowMapDepthTexture());
			glUniform1i(ShadowMapSampler_loc, (GLuint)TextureUnit::ShadowTextureUnit);
		}

		// the bias for depthMVP to map the NDC coordinate from [-1,1] to [0,1] which is a necessity for texture sampling
		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);

		glm::mat4 depthBiasMVP = biasMatrix * pShadowRender->GetShadowMapDepthVP() * pOwnerObject->getTransMatrix();
		glUniformMatrix4fv(depthBiasMVP_loc, 1, GL_FALSE, &depthBiasMVP[0][0]);
			
		glm::vec3 EyePosition = pWorld->pSceneMgr->pCamera->EyePosition;
		glUniform3f(Eyepos_loc, EyePosition.x, EyePosition.y, EyePosition.z);

		glUniform1i(texflag_loc, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		glBindVertexArray(pOwnerObject->GetVAO());

		glBindBuffer(GL_ARRAY_BUFFER, pOwnerObject->GetVertexCoordsVBO());
		glEnableVertexAttribArray(static_cast<GLuint>(Location::VERTEX_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::VERTEX_LOCATION), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Vertex position

		glBindBuffer(GL_ARRAY_BUFFER, pOwnerObject->GetTexCoordsVBO());
		glEnableVertexAttribArray(static_cast<GLuint>(Location::TEXTURE_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::TEXTURE_LOCATION), 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Texcoords

		glBindBuffer(GL_ARRAY_BUFFER, pOwnerObject->GetNormalsVBO());
		glEnableVertexAttribArray(static_cast<GLuint>(Location::NORMAL_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::NORMAL_LOCATION), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Normal

																														   // Set model matrix
		glUniformMatrix4fv(M_MatrixID, 1, GL_FALSE, &pOwnerObject->getTransMatrix()[0][0]);

		// Bind the material of the mesh
		pMesh->BindMaterial(Ka_loc, Kd_loc, Ks_loc, TexSamplerHandle);

		// Draw the mesh
		pMesh->DrawMesh();
	}
	
	void ShadowedMeshRenderPipeline::PostRender(std::shared_ptr<Mesh> pMesh)
	{

	}

	ShadowMapCookingPipeline::ShadowMapCookingPipeline()
		: RenderPipeline()
	{
		PipelineName = "ShadowDepthTexturePipeline";
	}

	ShadowMapCookingPipeline::~ShadowMapCookingPipeline()
	{

	}

	void ShadowMapCookingPipeline::RenderShadowsToTexture(std::shared_ptr<Mesh> pMesh)
	{
		auto pRender = pOwnerRender.lock();
		ShadowMapRender* pShadowRender = dynamic_cast<ShadowMapRender*>(pRender.get());
		if (!pShadowRender)
			return;

		auto pOwnerObject = pMesh->GetOwnerObject();
		if (pOwnerObject->isEnable() || !pOwnerObject->isReceiveShadows())
			return;

		glViewport(0, 0, pShadowRender->GetShadowMapWidth(), pShadowRender->GetShadowMapWidth());

		GLuint depthMVP_Loc = glGetUniformLocation(ProgramID, "depthMVP");

		glm::mat4 depthMVP;

		glBindVertexArray(pOwnerObject->GetVAO());

		glBindBuffer(GL_ARRAY_BUFFER, pOwnerObject->GetVertexCoordsVBO());
		glEnableVertexAttribArray(static_cast<GLuint>(Location::VERTEX_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::VERTEX_LOCATION), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Vertex position

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pOwnerObject->GetEBO());

		// MeshRender depth map of the shape
		depthMVP = pShadowRender->GetShadowMapDepthVP() * pOwnerObject->getTransMatrix();

		glUniformMatrix4fv(depthMVP_Loc, 1, GL_FALSE, &depthMVP[0][0]);

		glDrawElements(GL_TRIANGLES, pMesh->GetMeshVertexIndices().size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	}

	void ShadowMapCookingPipeline::CookShadowMapDepthTexture(std::shared_ptr<Mesh> pMesh)
	{
		auto pRender = pOwnerRender.lock();
		ShadowMapRender* pShadowRender = dynamic_cast<ShadowMapRender*>(pRender.get());
		if (!pShadowRender)
			return;

		glBindFramebuffer(GL_FRAMEBUFFER, pShadowRender->GetShadowMapFBO());
		glViewport(0, 0, pShadowRender->GetShadowMapWidth(), pShadowRender->GetShadowMapHeight());
		auto CubeMapIterator = pShadowRender->GetCubeMapPose();

		if (pShadowRender->GetLightInfo()->GetLightType() == eLightType::OmniDirectional) {
			// Clear the six face of the cube map for the next rendering
			for (uint16_t i = 0; i < 6; i++) {
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, CubeMapIterator[i].CubeMapFace, pShadowRender->GetShadowCubeMap(), 0);
				glClear(GL_DEPTH_BUFFER_BIT);
			}
		}

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		if (pShadowRender->GetLightInfo()->GetLightType() == eLightType::OmniDirectional)
		{
			// Draw 6 faces of cube map
			for (uint16_t k = 0; k < 6; k++)
			{

				// Draw shadow of one face into the cube map 
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, CubeMapIterator[k].CubeMapFace, pShadowRender->GetShadowCubeMap(), 0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, pShadowRender->GetShadowCubeMap());
				// Set the depth matrix correspondingly
				pShadowRender->SetLightSpaceMatrix(glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f),
					glm::lookAt(pShadowRender->GetLightInfo()->GetLightPos(), pShadowRender->GetLightInfo()->GetLightPos() + CubeMapIterator[k].Direction, CubeMapIterator[k].UpVector));
			}
		}
		else {
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		// Render the shadows to the depth texture
		RenderShadowsToTexture(pMesh);
	}

	void ShadowMapCookingPipeline::PreRender(std::shared_ptr<Mesh> pMesh)
	{
	
	}

	void ShadowMapCookingPipeline::Render(std::shared_ptr<Mesh> pMesh)
	{
		// Cook shadow depth texture
		CookShadowMapDepthTexture(pMesh);
	}

	void ShadowMapCookingPipeline::PostRender(std::shared_ptr<Mesh> pMesh)
	{
		
	}
}