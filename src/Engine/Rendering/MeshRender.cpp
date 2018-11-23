#include "MeshRender.h"

#ifdef WIN32

#include "..\Scene\SceneManager.h"

#else

#include "../Scene/SceneManager.h"

#endif

namespace cxc
{
	MeshRender::MeshRender():
		bIsRenderActive(false)
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

	void MeshRender::PreRender(std::shared_ptr<Mesh> pMesh)
	{
		for (auto pRenderPipeline : pRenderPipelines)
		{
			if (pRenderPipeline.second)
			{
				BindCameraUniforms(pRenderPipeline.second->GetPipelineProgramID());
				pRenderPipeline.second->UsePipeline();
				pRenderPipeline.second->PreRender(pMesh);
				CurrentUsedPipeline = pRenderPipeline.second;
			}
		}
	}

	void MeshRender::Render(std::shared_ptr<Mesh> pMesh)
	{
		for (auto pRenderPipeline : pRenderPipelines)
		{
			if (pRenderPipeline.second)
			{
				BindCameraUniforms(pRenderPipeline.second->GetPipelineProgramID());
				pRenderPipeline.second->UsePipeline();
				pRenderPipeline.second->Render(pMesh);
				CurrentUsedPipeline = pRenderPipeline.second;
			}
		}
	}

	void MeshRender::PostRender(std::shared_ptr<Mesh> pMesh)
	{
		for (auto pRenderPipeline : pRenderPipelines)
		{
			if (pRenderPipeline.second)
			{
				BindCameraUniforms(pRenderPipeline.second->GetPipelineProgramID());
				pRenderPipeline.second->UsePipeline();
				pRenderPipeline.second->PostRender(pMesh);
				CurrentUsedPipeline = pRenderPipeline.second;
			}
		}
	}

	void MeshRender::AddLight(const glm::vec3 &pos, const glm::vec3 &dir, eLightType type, eInteractionType interactive)
	{
		pLightInfo = std::make_shared<BaseLighting>(pos, dir, type, interactive);
	}

	void MeshRender::UsePipeline(const std::string& PipelineName)
	{
		auto TargetPipeline = GetPipelinePtr(PipelineName);
		if (TargetPipeline)
		{
			TargetPipeline->UsePipeline();
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
		DepthProjectionMatrix(1.0f), DepthViewMatrix(1.0f), DepthVP(1.0f)
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

	bool ShadowMapRender::InitShadowMapRender() noexcept
	{
		// Create framebuffer object for rendering
		glGenFramebuffers(1, &FrameBufferObjectID);
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferObjectID);

		if (!pLightInfo)
			return false;

		auto Type = pLightInfo->GetLightType();

		if (Type == eLightType::Directional ||
			Type == eLightType::Spot)
		{
			// Two pass shadow map
			if (Type == eLightType::Directional) {
				// Parallel light
				DepthProjectionMatrix = glm::ortho<float>(-100, 100, -100, 100, -100, 200);
				DepthViewMatrix = glm::lookAt(pLightInfo->GetLightPos(), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
				DepthVP = DepthProjectionMatrix * DepthViewMatrix;
			}
			else
			{
				// Spot Light
				DepthProjectionMatrix = glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 1000.0f);
				DepthViewMatrix = glm::lookAt(pLightInfo->GetLightPos(), pLightInfo->GetLightPos() + pLightInfo->GetDirection(), glm::vec3(0, -1, 0));
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
		else if (Type == eLightType::OmniDirectional)
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

		return true;
	}

	void ShadowMapRender::PreRender(std::shared_ptr<Mesh> pMesh)
	{
		// Shadow depth map cooked in the pre render process
		auto ShadowMapPipeline = GetPipelinePtr("ShadowDepthTexturePipeline");
		// Cook the shadow depth map
		ShadowMapPipeline->UsePipeline();
		ShadowMapPipeline->PreRender(pMesh);
		ShadowMapPipeline->Render(pMesh);
		ShadowMapPipeline->PostRender(pMesh);
	}

	void ShadowMapRender::Render(std::shared_ptr<Mesh> pMesh)
	{
		// Mesh is rendered in the render process
		auto SceneRenderingPipeline = GetPipelinePtr("ShadowedMeshRenderPipeline");

		SceneRenderingPipeline->UsePipeline();
		BindCameraUniforms(SceneRenderingPipeline->GetPipelineProgramID());
		SceneRenderingPipeline->PreRender(pMesh);
		SceneRenderingPipeline->Render(pMesh);
		SceneRenderingPipeline->PostRender(pMesh);
	}

	void ShadowMapRender::PostRender(std::shared_ptr<Mesh> pMesh)
	{

	}

	bool ShadowMapRender::InitializeRender()
	{
		bool bSuccessful = MeshRender::InitializeRender();
		bSuccessful &= InitShadowMapRender();
		return bSuccessful;
	}
}