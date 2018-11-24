#include "MeshRender.h"

#ifdef WIN32

#include "..\Scene\SceneManager.h"

#else

#include "../Scene/SceneManager.h"

#endif

namespace cxc
{
	MeshRender::MeshRender()
	{

	}

	MeshRender::MeshRender(const std::string& Name)
		: MeshRender()
	{
		RenderName = Name;
	}

	MeshRender::~MeshRender()
	{

	}

	bool MeshRender::InitializeRender()
	{
		bool bSuccessful = true;
		// Initialize all the pipelines
		for (auto pPipeline : pRenderPipelines)
		{
			if (pPipeline.second)
				bSuccessful &= pPipeline.second->InitializePipeline();
		}

		return bSuccessful;
	}

	void MeshRender::PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{

	}

	void MeshRender::Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{
		// Determing which pipeline should be used to render the material
		bool bHasTexture = false;
		if (pMesh->GetMeshMaterial())
		{
			bHasTexture = pMesh->GetMeshMaterial()->pTextures.size() > 0;
		}
		// Switch pipeline to render the material that has textures
		auto pRenderMgr = RenderManager::GetInstance();
		if (bHasTexture)
		{
			auto TexturingPipeline = GetPipelinePtr("TexturingPipeline");
			auto PipelineUsed = CurrentUsedPipeline != TexturingPipeline ? TexturingPipeline : CurrentUsedPipeline;
			
			BindCameraUniforms(PipelineUsed->GetPipelineProgramID());
			UsePipeline("TexturingPipeline");
			PipelineUsed->Render(pMesh, Lights);
		}
		else
		{
			auto NontexturingPipeline = GetPipelinePtr("NontexturingPipeline");
			auto PipelineUsed = CurrentUsedPipeline != NontexturingPipeline ? NontexturingPipeline : CurrentUsedPipeline;
			BindCameraUniforms(PipelineUsed->GetPipelineProgramID());
			UsePipeline("NontexturingPipeline");
			PipelineUsed->Render(pMesh, Lights);
		}	
	}

	void MeshRender::PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{
		
	}

	void MeshRender::UsePipeline(const std::string& PipelineName)
	{
		auto TargetPipeline = GetPipelinePtr(PipelineName);
		if (TargetPipeline)
		{
			glUseProgram(TargetPipeline->GetPipelineProgramID());
			CurrentUsedPipeline = TargetPipeline;
		}
	}

	void MeshRender::BindCameraUniforms(GLuint ProgramID)
	{
		auto SceneManager = SceneManager::GetInstance();
		SceneManager->pCamera->BindCameraUniforms(ProgramID);
	}

	void MeshRender::AddRenderingPipeline(std::shared_ptr<RenderPipeline> pPipeline)
	{
		pPipeline->SetOwnerRender(shared_from_this());

		pRenderPipelines.insert(std::make_pair(pPipeline->GetPipelineName(), pPipeline));
	}

	std::shared_ptr<RenderPipeline> MeshRender::GetPipelinePtr(const std::string& PipelineName)
	{
		auto iter = pRenderPipelines.find(PipelineName);
		if (iter != pRenderPipelines.end())
		{
			return iter->second;
		}
		else
			return nullptr;
	}

	ShadowMapRender::ShadowMapRender():
		FrameBufferObjectID(0), DepthMapTexture(0),
		DepthMapWidth(1024), DepthMapHeight(1024), ShadowCubeMap(0),
		DepthProjectionMatrix(1.0f), DepthViewMatrix(1.0f), DepthVP(1.0f),
		bIsShadowTextureCreate(false)
	{
		CubeMapIterator[0] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1.0f,0.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f) };
		CubeMapIterator[1] = { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.0f,0.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f) };
		CubeMapIterator[2] = { GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.0f,1.0f,0.0f), glm::vec3(0.0f,0.0f,-1.0f) };
		CubeMapIterator[3] = { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.0f,-1.0f,0.0f), glm::vec3(0.0f,0.0f,-1.0f) };
		CubeMapIterator[4] = { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0.0f,0.0f,1.0f), glm::vec3(0.0f,-1.0f,0.0f) };
		CubeMapIterator[5] = { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0.0f,0.0f,-1.0f), glm::vec3(0.0f,-1.0f,0.0f) };
	}

	ShadowMapRender::ShadowMapRender(const std::string& Name):
		ShadowMapRender()
	{
		SetRenderName(Name);
	}

	ShadowMapRender::~ShadowMapRender()
	{
		if (FrameBufferObjectID)
		{
			glDeleteFramebuffers(1, &FrameBufferObjectID);
		}

		if (DepthMapTexture)
		{
			glDeleteTextures(1, &DepthMapTexture);
		}
	}

	void ShadowMapRender::SetShadowMapTextureSize(uint32_t Height, uint32_t Width)
	{
		if (Height == Width)
		{
			DepthMapHeight = Height;
			DepthMapWidth = Width;
		}
		else
		{
			std::cerr << "DepthMap height must equal to the width " << std::endl;
		}
	}

	void ShadowMapRender::SetLightSpaceMatrix(const glm::mat4 &Projection, const glm::mat4 &View) noexcept
	{
		DepthProjectionMatrix = Projection;
		DepthViewMatrix = View;

		DepthVP = Projection * View;
	}

	GLuint ShadowMapRender::GetShadowMapFBO() const noexcept
	{
		return FrameBufferObjectID;
	}

	GLuint ShadowMapRender::GetShadowMapWidth() const noexcept
	{
		return DepthMapWidth;
	}

	GLuint ShadowMapRender::GetShadowMapHeight() const noexcept
	{
		return DepthMapHeight;
	}

	glm::mat4 ShadowMapRender::GetShadowMapDepthVP() const noexcept
	{
		return DepthVP;
	}

	GLuint ShadowMapRender::GetShadowMapDepthTexture() const noexcept
	{
		return DepthMapTexture;
	}

	GLuint ShadowMapRender::GetShadowCubeMap() const noexcept
	{
		return ShadowCubeMap;
	}

	bool ShadowMapRender::InitShadowMapRender(const std::vector<std::shared_ptr<BaseLighting>>& Lights) noexcept
	{
		if (!bIsShadowTextureCreate)
			return true;

		if (!Lights.empty())
			return false;

		auto pLight = Lights[0];
		if (!pLight)
			return false;

		// Create framebuffer object for rendering
		glGenFramebuffers(1, &FrameBufferObjectID);
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferObjectID);

		if (pLight->GetLightType() == eLightType::Directional ||
			pLight->GetLightType() == eLightType::Spot)
		{
			// Two pass shadow map
			if (pLight->GetLightType() == eLightType::Directional) {
				// Parallel light
				DepthProjectionMatrix = glm::ortho<float>(-100, 100, -100, 100, -100, 200);
				DepthViewMatrix = glm::lookAt(pLight->GetLightPos(), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
				DepthVP = DepthProjectionMatrix * DepthViewMatrix;
			}
			else
			{
				// Spot Light
				DepthProjectionMatrix = glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 1000.0f);
				DepthViewMatrix = glm::lookAt(pLight->GetLightPos(), pLight->GetLightPos() + pLight->GetDirection(), glm::vec3(0, -1, 0));
				DepthVP = DepthProjectionMatrix * DepthViewMatrix;
			}

			// Depth texture
			glGenTextures(1, &DepthMapTexture);
			glBindTexture(GL_TEXTURE_2D, DepthMapTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, DepthMapWidth,
				DepthMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

			// Attach the depth texture to the depth attachment point
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthMapTexture, 0);

			// Disable the color output and input
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
		else if (pLight->GetLightType() == eLightType::OmniDirectional)
		{
			// Multipass shadow map with Point light
			glEnable(GL_TEXTURE_CUBE_MAP);

			// Create Cube map for multipass
			glGenTextures(1, &ShadowCubeMap);
			glBindTexture(GL_TEXTURE_CUBE_MAP, ShadowCubeMap);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			for (uint16_t i = 0; i < 6; i++) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32, DepthMapWidth, DepthMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			}

			glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, ShadowCubeMap, 0);

			glDrawBuffer(GL_NONE);
		}

		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (Status != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("Framebuffer error, status: 0x%x\n", Status);

			if (FrameBufferObjectID)
				glDeleteFramebuffers(1, &FrameBufferObjectID);
			if (DepthMapTexture)
				glDeleteTextures(1, &DepthMapTexture);

			return false;
		}

		bIsShadowTextureCreate = true;

		return true;
	}

	void ShadowMapRender::PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{
		InitShadowMapRender(Lights);
		
		// Shadow depth map cooked in the pre-render process
		auto ShadowMapPipeline = GetPipelinePtr("ShadowDepthTexturePipeline");

		// Cook the shadow depth map
		UsePipeline("ShadowDepthTexturePipeline");
		ShadowMapPipeline->PreRender(pMesh, Lights);
		ShadowMapPipeline->Render(pMesh, Lights);
		ShadowMapPipeline->PostRender(pMesh, Lights);
	}

	void ShadowMapRender::Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{
		// Mesh is rendered in the render process
		auto SceneRenderingPipeline = GetPipelinePtr("ShadowedMeshRenderPipeline");

		UsePipeline("ShadowedMeshRenderPipeline");
		BindCameraUniforms(SceneRenderingPipeline->GetPipelineProgramID());
		SceneRenderingPipeline->PreRender(pMesh, Lights);
		SceneRenderingPipeline->Render(pMesh, Lights);
		SceneRenderingPipeline->PostRender(pMesh, Lights);
	}

	void ShadowMapRender::PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{

	}
}