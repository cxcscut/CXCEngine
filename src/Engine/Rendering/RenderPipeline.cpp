#include "RenderPipeline.h"

#ifdef WIN32

#include "..\Scene\Object3D.h"
#include "..\World\World.h"
#include "..\Scene\SceneManager.h"
#include "..\Scene\Lighting.h"

#else

#include "../Scene/Object3D.h"
#include "../Scene/SceneManager.h"
#include "../World/World.h"
#include "../Scene/Lighting.h"

#endif

namespace cxc
{
	RenderPipeline::RenderPipeline()
	{
		pShaders = std::vector<std::shared_ptr<Shader>>(static_cast<size_t>(eShaderType::SHADER_TYPESIZE), nullptr);
		ProgramID = glCreateProgram();
		pOwnerRender.reset();
	}

	RenderPipeline::RenderPipeline(const std::string& Name):
		RenderPipeline()
	{
		PipelineName = Name;
	}

	RenderPipeline::~RenderPipeline()
	{
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

	bool RenderPipeline::CheckLinkingStatus(std::string& OutResultLog) const
	{
		GLint Result;
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);

		GLint LogLen = 0;
		OutResultLog.clear();
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &LogLen);
		if (LogLen > 0)
		{
			char Log[1024];
			GLint nLength;
			glGetProgramInfoLog(ProgramID, 1024, &nLength, Log);
			OutResultLog = Log;
		}

		return Result > 0 ? true : false;
	}

	bool RenderPipeline::InitializePipeline()
	{
		bool bSuccessful = true;
		
		// Link shaders
		bSuccessful &= LinkShaders();

		return bSuccessful;
	}

	void RenderPipeline::AttachShader(std::shared_ptr<Shader> pShader)
	{
		if (pShader && pShader->IsCompiled())
		{
			// Detach first
			DetachShader(pShader->GetShaderType());

			// Attach shader and link the program
			glAttachShader(ProgramID, pShader->GetShaderID());

			pShaders[static_cast<size_t>(pShader->GetShaderType())] = pShader;
		}
	}

	void RenderPipeline::DetachShader(eShaderType AttachmentLoc)
	{
		auto ShaderDetaching = pShaders[static_cast<size_t>(AttachmentLoc)];
		if (ShaderDetaching)
		{
			glDetachShader(ProgramID, ShaderDetaching->GetShaderID());
		}

		pShaders[static_cast<size_t>(AttachmentLoc)] = nullptr;
	}

	bool RenderPipeline::LinkShaders()
	{
		// Link program
		glLinkProgram(ProgramID);

		std::string OutDebugLog;
		bool bResult = CheckLinkingStatus(OutDebugLog);
		if (!bResult)
		{
			std::cerr << "Failed to link the program" << std::endl;
			std::cerr << OutDebugLog << std::endl;
		}

		return bResult;
	}

	std::shared_ptr<Shader> RenderPipeline::GetCurrentAttachedShader(eShaderType ShaderType)
	{
		return pShaders[static_cast<size_t>(ShaderType)];
	}

	void RenderPipeline::PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{
		// Do noting in the base render class
	}

	void RenderPipeline::Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{
		GLint TexSamplerHandle;
		GLint Eyepos_loc, M_MatrixID;
		GLint Ka_loc, Ks_loc, Kd_loc;

		auto pWorld = World::GetInstance();
		auto pRender = pOwnerRender.lock();
		auto pOwnerObject = pMesh->GetOwnerObject();

		if (Lights.empty())
			return;

		auto pLight = Lights[0];
		if (!pLight)
			return;

		// Determing which fragment shader should be used to render the material
		bool bHasTexture = false;
		if (pMesh->GetMeshMaterial())
		{
			bHasTexture = pMesh->GetMeshMaterial()->pTextures.size() > 0;
		}

		// Switch framgment shader to render the material that has textures
		auto CurrentAttachedFS = GetCurrentAttachedShader(eShaderType::FRAGMENT_SHADER);
		auto pRenderMgr = RenderManager::GetInstance();
		if (bHasTexture)
		{
			if (!CurrentAttachedFS || CurrentAttachedFS->GetShaderName() != "PhongFSWithTexture")
			{
				auto TexturingFS = pRenderMgr->GetShader("PhongFSWithTexture");
				if (TexturingFS)
				{
					// Replace the texture fragment shader
					AttachShader(TexturingFS);

					// Link the program
					if (!LinkShaders())
					{
						std::cerr << "Failed to link the shaders when switching to PhongFSWithTexture" << std::endl;
						return;
					}
				}
				else
				{
					std::cerr << "Can not find PhongFSWithTexture shader" << std::endl;
					return;
				}
			}
		}
		else
		{
			if (!CurrentAttachedFS || CurrentAttachedFS->GetShaderName() != "PhongFSWithNoTexture")
			{
				// Detach the shader
				DetachShader(eShaderType::FRAGMENT_SHADER);
				auto NoTexturingFS = pRenderMgr->GetShader("PhongFSWithNoTexture");
				if (NoTexturingFS)
				{
					// Attach the non-texture fragment shader
					AttachShader(NoTexturingFS);

					// Link the program
					if (!LinkShaders())
					{
						std::cerr << "Failed to link the shaders when switching to PhongFSWithNoTexture" << std::endl;
						return;
					}
				}
				else
				{
					std::cerr << "Can not find PhongFSWithNoTexture shader" << std::endl;
					return;
				}
			}
		}

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, pWorld->pWindowMgr->GetWindowWidth(), pWorld->pWindowMgr->GetWindowHeight());
		BindLightUniforms(pLight);

		TexSamplerHandle = glGetUniformLocation(ProgramID, "TexSampler");
		M_MatrixID = glGetUniformLocation(ProgramID, "M");
		Ka_loc = glGetUniformLocation(ProgramID, "Ka");
		Ks_loc = glGetUniformLocation(ProgramID, "Ks");
		Kd_loc = glGetUniformLocation(ProgramID, "Kd");


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

	void RenderPipeline::PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{
		// Do noting in the base render class
	}

	ShadowedMeshRenderPipeline::ShadowedMeshRenderPipeline() :
		RenderPipeline("ShadowedMeshRenderPipeline")
	{

	}

	ShadowedMeshRenderPipeline::~ShadowedMeshRenderPipeline()
	{

	}

	void ShadowedMeshRenderPipeline::PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{

	}

	void ShadowedMeshRenderPipeline::Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{
		GLint TexSamplerHandle, depthBiasMVP_loc;
		GLint ShadowMapSampler_loc, Eyepos_loc, M_MatrixID;
		GLint Ka_loc, Ks_loc, Kd_loc, isPointLight_loc;
		GLint shadowmapCube_loc;

		if (!Lights.empty())
			return;

		auto pLight = Lights[0];
		if (!pLight)
			return;

		auto pWorld = World::GetInstance();
		auto pRender = pOwnerRender.lock();
		auto pShadowRender = dynamic_cast<ShadowMapRender*>(pRender.get());
		if (!pShadowRender)
			return;

		auto pOwnerObject = pMesh->GetOwnerObject();

		// Determing which fragment shader should be used to render the material
		bool bHasTexture = false;
		if (pMesh->GetMeshMaterial())
		{
			bHasTexture = pMesh->GetMeshMaterial()->pTextures.size() > 0;
		}

		// Switch framgment shader to render the material that has textures
		auto CurrentAttachedFS = GetCurrentAttachedShader(eShaderType::FRAGMENT_SHADER);
		auto pRenderMgr = RenderManager::GetInstance();
		if (bHasTexture)
		{
			if (!CurrentAttachedFS || CurrentAttachedFS->GetShaderName() != "PhongFSWithTexture")
			{
				// Replace the shader
				auto TexturingFS = pRenderMgr->GetShader("PhongFSWithTexture");
				if (TexturingFS)
				{
					// Attach the texture fragment shader
					AttachShader(TexturingFS);

					// Link the program
					if (!LinkShaders())
					{
						std::cerr << "Failed to link the shaders when switching to PhongFSWithTexture" << std::endl;
						return;
					}
				}
				else
				{
					std::cerr << "Can not find PhongFSWithTexture shader" << std::endl;
					return;
				}
			}
		}
		else
		{
			if (!CurrentAttachedFS || CurrentAttachedFS->GetShaderName() != "PhongFSWithNoTexture")
			{
				// Detach the shader
				DetachShader(eShaderType::FRAGMENT_SHADER);
				auto NoTexturingFS = pRenderMgr->GetShader("PhongFSWithNoTexture");
				if (NoTexturingFS)
				{
					// Attach the non-texture fragment shader
					AttachShader(NoTexturingFS);

					// Link the program
					if (!LinkShaders())
					{
						std::cerr << "Failed to link the shaders when switching to PhongFSWithNoTexture" << std::endl;
						return;
					}
				}
				else
				{
					std::cerr << "Can not find PhongFSWithNoTexture shader" << std::endl;
					return;
				}
			}
		}

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, pWorld->pWindowMgr->GetWindowWidth(), pWorld->pWindowMgr->GetWindowHeight());
		BindLightUniforms(pLight);

		TexSamplerHandle = glGetUniformLocation(ProgramID, "TexSampler");
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
		if (pLight->GetLightType() == eLightType::OmniDirectional)
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
	
	void ShadowedMeshRenderPipeline::PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
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

	void ShadowMapCookingPipeline::RenderShadowsToTexture(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
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

	void ShadowMapCookingPipeline::CookShadowMapDepthTexture(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{
		auto pRender = pOwnerRender.lock();
		ShadowMapRender* pShadowRender = dynamic_cast<ShadowMapRender*>(pRender.get());
		if (!pShadowRender || !Lights.empty())
			return;

		auto pLight = Lights[0];
		if (!pLight)
			return;

		glBindFramebuffer(GL_FRAMEBUFFER, pShadowRender->GetShadowMapFBO());
		glViewport(0, 0, pShadowRender->GetShadowMapWidth(), pShadowRender->GetShadowMapHeight());
		auto CubeMapIterator = pShadowRender->GetCubeMapPose();

		if (pLight->GetLightType() == eLightType::OmniDirectional) {
			// Clear the six face of the cube map for the next rendering
			for (uint16_t i = 0; i < 6; i++) {
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, CubeMapIterator[i].CubeMapFace, pShadowRender->GetShadowCubeMap(), 0);
				glClear(GL_DEPTH_BUFFER_BIT);
			}
		}

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		if (pLight->GetLightType() == eLightType::OmniDirectional)
		{
			// Draw 6 faces of cube map
			for (uint16_t k = 0; k < 6; k++)
			{

				// Draw shadow of one face into the cube map 
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, CubeMapIterator[k].CubeMapFace, pShadowRender->GetShadowCubeMap(), 0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, pShadowRender->GetShadowCubeMap());
				// Set the depth matrix correspondingly
				pShadowRender->SetLightSpaceMatrix(glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f),
					glm::lookAt(pLight->GetLightPos(), pLight->GetLightPos() + CubeMapIterator[k].Direction, CubeMapIterator[k].UpVector));
			}
		}
		else {
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		// Render the shadows to the depth texture
		RenderShadowsToTexture(pMesh, Lights);
	}

	void ShadowMapCookingPipeline::PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{
	
	}

	void ShadowMapCookingPipeline::Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{
		// Cook shadow depth texture
		CookShadowMapDepthTexture(pMesh, Lights);
	}

	void ShadowMapCookingPipeline::PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{
		
	}
}