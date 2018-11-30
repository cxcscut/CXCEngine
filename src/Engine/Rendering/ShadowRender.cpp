#include "ShadowRender.h"
#include "RenderManager.h"

#if WIN32

#include "..\Scene\Mesh.h"

#else

#include "../Scene/Mesh.h"

#endif

namespace cxc
{
	ShadowRender::ShadowRender() :
		FrameBufferObjectID(0), DepthMapTexture(0),
		DepthMapSize(128), ShadowCubeMap(0),
		DepthProjectionMatrix(1.0f), DepthViewMatrix(1.0f), DepthVP(1.0f),
		bIsShadowTextureCreate(false)
	{
		CubeMapIterator[0] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) };
		CubeMapIterator[1] = { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) };
		CubeMapIterator[2] = { GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) };
		CubeMapIterator[3] = { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) };
		CubeMapIterator[4] = { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f) };
		CubeMapIterator[5] = { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f) };
	}

	ShadowRender::ShadowRender(const std::string& Name) :
		ShadowRender()
	{
		SetRenderName(Name);
	}

	ShadowRender::~ShadowRender()
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

	void ShadowRender::SetShadowMapResolution(GLuint Size)
	{
		DepthMapSize = Size;
	}

	void ShadowRender::SetLightSpaceMatrix(const glm::mat4 &Projection, const glm::mat4 &View) noexcept
	{
		DepthProjectionMatrix = Projection;
		DepthViewMatrix = View;

		DepthVP = Projection * View;
	}

	GLuint ShadowRender::GetShadowMapFBO() const noexcept
	{
		return FrameBufferObjectID;
	}

	GLuint ShadowRender::GetShadowMapSize() const noexcept
	{
		return DepthMapSize;
	}

	glm::mat4 ShadowRender::GetShadowMapDepthVP() const noexcept
	{
		return DepthVP;
	}

	GLuint ShadowRender::GetShadowMapDepthTexture() const noexcept
	{
		return DepthMapTexture;
	}

	GLuint ShadowRender::GetShadowCubeMap() const noexcept
	{
		return ShadowCubeMap;
	}

	bool ShadowRender::InitShadowMapRender(const std::vector<std::shared_ptr<LightSource>>& Lights) noexcept
	{
		if (bIsShadowTextureCreate)
			return true;

		if (Lights.empty())
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
			if (pLight->GetLightType() == eLightType::Directional)
			{
				// Parallel light
				DepthProjectionMatrix = glm::ortho<float>(-100, 100, -100, 100, -100, 200);
				DepthViewMatrix = glm::lookAt(pLight->GetLightPos(), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
				DepthVP = DepthProjectionMatrix * DepthViewMatrix;
			}
			else
			{
				// Spot Light
				DepthProjectionMatrix = glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 1000.0f);
				DepthViewMatrix = glm::lookAt(pLight->GetLightPos(), pLight->GetLightPos() + pLight->GetDirection(), glm::vec3(0, 0, -1));
				DepthVP = DepthProjectionMatrix * DepthViewMatrix;
			}

			// Depth texture
			glGenTextures(1, &DepthMapTexture);
			glBindTexture(GL_TEXTURE_2D, DepthMapTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, DepthMapSize,
				DepthMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
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
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32, DepthMapSize, DepthMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
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

	void ShadowRender::PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{
		auto pLight = Lights[0];
		bool bResult = InitShadowMapRender(Lights);
		if (!bResult)
		{
			std::cerr << "Failed to initialize the shadow map render" << std::endl;
			return;
		}

		if (pLight->GetLightType() == eLightType::OmniDirectional)
		{
			// Bind the shadowmap framebuffer switched from the system framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, FrameBufferObjectID);

			// Clear the six face of the cube map for the next rendering
			for (uint16_t i = 0; i < 6; i++) {
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, CubeMapIterator[i].CubeMapFace, ShadowCubeMap, 0);
				glClear(GL_DEPTH_BUFFER_BIT);
			}
		}
		else
		{
			glClear(GL_DEPTH_BUFFER_BIT);
		}
	}

	void ShadowRender::Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{
		// Shadow depth map cooked in the pre-render process
		auto ShadowMapPipeline = GetPipelinePtr("ShadowDepthTexturePipeline");

		// Cook the shadow depth map
		UsePipeline(ShadowMapPipeline);
		ShadowMapPipeline->PreRender(pMesh, Lights);
		ShadowMapPipeline->Render(pMesh, Lights);
		ShadowMapPipeline->PostRender(pMesh, Lights);
	}

	void ShadowRender::PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{
		// Determing which pipeline should be used to render the material
		bool bHasTexture = false;
		if (pMesh->GetMeshMaterial())
		{
			bHasTexture = pMesh->GetMeshMaterial()->pTextures.size() > 0;
		}

		// Switch pipeline to render the material that has textures or do not has textures
		auto pRenderMgr = RenderManager::GetInstance();
		if (bHasTexture)
		{
			// Texturing pipeline is used in rendering process
			auto TexturingShadowedMeshPipeline = GetPipelinePtr("TexturingShadowedMeshPipeline");

			UsePipeline(TexturingShadowedMeshPipeline);
			BindCameraUniforms(TexturingShadowedMeshPipeline->GetPipelineProgramID());
			TexturingShadowedMeshPipeline->PreRender(pMesh, Lights);
			TexturingShadowedMeshPipeline->Render(pMesh, Lights);
			TexturingShadowedMeshPipeline->PostRender(pMesh, Lights);
		}
		else
		{
			// Non-texturing pipline is used in the rendering process
			auto  NonTexturingShadowedMeshPipeline = GetPipelinePtr("NonTexturingShadowedMeshPipeline");

			UsePipeline(NonTexturingShadowedMeshPipeline);
			BindCameraUniforms(NonTexturingShadowedMeshPipeline->GetPipelineProgramID());
			NonTexturingShadowedMeshPipeline->PreRender(pMesh, Lights);
			NonTexturingShadowedMeshPipeline->Render(pMesh, Lights);
			NonTexturingShadowedMeshPipeline->PostRender(pMesh, Lights);
		}
	}
}