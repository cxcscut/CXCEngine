#include "RenderManager.h"

namespace cxc {

	RenderManager::RenderManager()
		:ShadowMapFBO(0), DepthTexture(0),
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

	RenderManager::~RenderManager()
	{
		if (ShadowMapFBO)
		{
			glDeleteFramebuffers(1, &ShadowMapFBO);
		}

		if (DepthTexture)
		{
			glDeleteTextures(1, &DepthTexture);
		}
	}

	void RenderManager::CreateLightInfo(const glm::vec3 &pos, const glm::vec3 &dir, eLightType type, eInteractionType interactive)
	{
		pLightInfo = std::make_shared<BaseLighting>(pos, dir, type, interactive);
	}

	void RenderManager::SetShadowMapSize(uint32_t Height, uint32_t Width)
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

	void RenderManager::SetTransformationMatrix(const glm::mat4 &Projection, const glm::mat4 &View) noexcept
	{
		DepthProjectionMatrix = Projection;
		DepthViewMatrix = View;

		DepthVP = Projection * View;
	}

	void RenderManager::SetLightPos(const glm::vec3 &pos) noexcept
	{
		if (pLightInfo)
			pLightInfo->SetLightPos(pos);
	}

	RenderManager::CubeMapCameraPose* RenderManager::GetShadowMapCameraPose() noexcept
	{
		return CubeMapIterator;
	}

	glm::vec3 RenderManager::GetLightPos() const noexcept
	{
		if (pLightInfo)
			return pLightInfo->GetLightPos();
		else
			return glm::vec3(0,0,0);
	}

	GLuint RenderManager::GetShadowMapFBO() const noexcept
	{
		return ShadowMapFBO;
	}

	GLuint RenderManager::GetShadowMapWidth() const noexcept
	{
		return DepthMapWidth;
	}

	GLuint RenderManager::GetShadowMapHeight() const noexcept
	{
		return DepthMapHeight;
	}

	glm::mat4 RenderManager::GetShadowMapDepthVP() const noexcept
	{
		return DepthVP;
	}

	GLuint RenderManager::GetShadowMapDepthTexture() const noexcept
	{
		return DepthTexture;
	}

	void RenderManager::SetLightType(eLightType type) noexcept
	{
		if (pLightInfo)
			pLightInfo->SetLightType(type);
	}


	eLightType RenderManager::GetLightType() const noexcept
	{
		if (pLightInfo)
			return pLightInfo->GetLightType();

		return eLightType::InvalidType;
	}

	GLuint RenderManager::GetShadowCubeMap() const noexcept
	{
		return ShadowCubeMap;
	}

	void RenderManager::SetCurrentUsedRender(std::shared_ptr<Render> pRender)
	{
		CurrentUsedRender = pRender;
	}

	bool RenderManager::InitShadowMapRender() noexcept
	{
		// Create framebuffer object for rendering
		glGenFramebuffers(1, &ShadowMapFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, ShadowMapFBO);

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
				DepthViewMatrix = glm::lookAt(pLightInfo->GetLightPos(),pLightInfo->GetLightPos() + pLightInfo->GetDirection(),glm::vec3(0,-1,0));
				DepthVP = DepthProjectionMatrix * DepthViewMatrix;
			}

			// Depth texture
			glGenTextures(1, &DepthTexture);
			glBindTexture(GL_TEXTURE_2D, DepthTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, DepthMapWidth,
				DepthMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

			// Attach the depth texture to the depth attachment point
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthTexture, 0);

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
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0,GL_DEPTH_COMPONENT32, DepthMapWidth, DepthMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			}

			glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, ShadowCubeMap, 0);

			glDrawBuffer(GL_NONE);
		}

		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (Status != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("Framebuffer error, status: 0x%x\n", Status);

			if (ShadowMapFBO)
				glDeleteFramebuffers(1, &ShadowMapFBO);
			if (DepthTexture)
				glDeleteTextures(1, &DepthTexture);

			return false;
		}
		
		return true;
	}

	std::shared_ptr<Render> RenderManager::GetRenderPtr(const std::string &name) noexcept
	{
		auto it = m_Renders.find(name);
		if (it != m_Renders.end())
			return it->second;
		else
			return nullptr;

	}

	void RenderManager::UseRender(const std::string& RenderName)
	{
		auto pRender = GetRenderPtr(RenderName);
		if (pRender)
		{
			CurrentUsedRender = pRender;
		}
	}

	void RenderManager::addRender(const std::string &name, std::shared_ptr<Render> pRender) noexcept
	{
		m_Renders.insert(std::make_pair(name, pRender));
	}

	void RenderManager::deleteRender(const std::string &name) noexcept
	{
		auto it = m_Renders.find(name);
		if (it != m_Renders.end())
			m_Renders.erase(it);
	}

}