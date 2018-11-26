#include "ShadowRenderPipeline.h"
#include "ShadowRender.h"

#if WIN32

#include "..\World\World.h"

#else

#include "../World/World.h"

#endif

namespace cxc
{
	ShadowedMeshRenderPipeline::ShadowedMeshRenderPipeline()
	{

	}

	ShadowedMeshRenderPipeline::ShadowedMeshRenderPipeline(const std::string& Name)
		: RenderPipeline(Name)
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
		GLint Ka_loc, Ks_loc, Kd_loc;
		GLint shadowmapCube_loc;
		GLint LightPowerLoc;

		if (Lights.empty())
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

		glUseProgram(ProgramID);
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

		shadowmapCube_loc = glGetUniformLocation(ProgramID, "shadowmapCube");
		depthBiasMVP_loc = glGetUniformLocation(ProgramID, "DepthBiasMVP");
		ShadowMapSampler_loc = glGetUniformLocation(ProgramID, "shadowmap");
		LightPowerLoc = glGetUniformLocation(ProgramID, "LightPower");
		GLint test_loc = glGetUniformLocation(ProgramID, "test");

		// Bind depth texture to the texture unit 1
		// We use texture unit 0 for the objectss texture sampling 
		// while texture unit 1 for depth buffer sampling
		glActiveTexture(GL_TEXTURE0 + (GLuint)TextureUnit::ShadowTextureUnit);
		if (pLight->GetLightType() == eLightType::OmniDirectional)
		{
			glBindTexture(GL_TEXTURE_CUBE_MAP, pShadowRender->GetShadowCubeMap());
			glUniform1i(shadowmapCube_loc, (GLuint)TextureUnit::ShadowTextureUnit);
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

		glUniform1f(LightPowerLoc, pLight->GetIntensity());

		glBindVertexArray(pOwnerObject->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->GetMeshEBO());

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
		if (!pOwnerObject->isEnable() || !pOwnerObject->isReceiveShadows())
			return;

		glViewport(0, 0, pShadowRender->GetShadowMapWidth(), pShadowRender->GetShadowMapWidth());

		GLint depthMVP_Loc = glGetUniformLocation(ProgramID, "depthMVP");

		glm::mat4 depthMVP;

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glBindVertexArray(pOwnerObject->GetVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pMesh->GetMeshEBO());

		glBindBuffer(GL_ARRAY_BUFFER, pOwnerObject->GetVertexCoordsVBO());
		glEnableVertexAttribArray(static_cast<GLuint>(Location::VERTEX_LOCATION));
		glVertexAttribPointer(static_cast<GLuint>(Location::VERTEX_LOCATION), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); // Vertex position

																														   // Rendering depth map of the mesh to depth texture
		depthMVP = pShadowRender->GetShadowMapDepthVP() * pOwnerObject->getTransMatrix();

		glUniformMatrix4fv(depthMVP_Loc, 1, GL_FALSE, &depthMVP[0][0]);

		// Draw mesh depth to the texture
		pMesh->DrawMesh();
	}

	void ShadowMapCookingPipeline::CookShadowMapDepthTexture(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{
		auto pRender = pOwnerRender.lock();
		ShadowMapRender* pShadowRender = dynamic_cast<ShadowMapRender*>(pRender.get());
		if (!pShadowRender || Lights.empty())
			return;

		auto pLight = Lights[0];
		if (!pLight)
			return;

		auto CubeMapIterator = pShadowRender->GetCubeMapPose();

		glBindFramebuffer(GL_FRAMEBUFFER, pShadowRender->GetShadowMapFBO());
		glViewport(0, 0, pShadowRender->GetShadowMapWidth(), pShadowRender->GetShadowMapHeight());
		if (pLight->GetLightType() == eLightType::OmniDirectional)
		{
			// Draw 6 faces of cube map
			for (uint16_t k = 0; k < 6; k++)
			{
				// Draw shadow of one face into the cube map 
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, CubeMapIterator[k].CubeMapFace, pShadowRender->GetShadowCubeMap(), 0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, pShadowRender->GetShadowCubeMap());

				// Set the depth matrix correspondingly, FOV of the projection matrix must be 90 degrees to capture the whole scene
				pShadowRender->SetLightSpaceMatrix(glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f),
					glm::lookAt(pLight->GetLightPos(), pLight->GetLightPos() + CubeMapIterator[k].Direction, CubeMapIterator[k].UpVector));

				// Render the shadows to the CubeMap
				RenderShadowsToTexture(pMesh, Lights);
			}
		}
		else
		{
			// Render the shadows to the Shadowmap
			RenderShadowsToTexture(pMesh, Lights);
		}
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