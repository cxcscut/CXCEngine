#include "RendererManager.h"

namespace cxc {

	BaseRender::BaseRender(const std::string &vertex_file_path, const std::string &fragment_file_path)
		:ProgramID(0),VertexShader(0),FragmentShader(0), 
		VertexShaderPath(vertex_file_path),FragmentShaderPath(fragment_file_path)
	{
	}

	BaseRender::~BaseRender()
	{
	}

	bool BaseRender::CreateShaderProgram()
	{
		VertexShader = glCreateShader(GL_VERTEX_SHADER);
		FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		ProgramID = glCreateProgram();

		std::string VertexShaderCode, FragmentShaderCode;

		if (!LoadVertexShader(VertexShaderPath, VertexShaderCode)) {
			std::cout << "VS load failed" << std::endl;
			return false;
		}

		if (!LoadFragmentShader(FragmentShaderPath, FragmentShaderCode)) {
			std::cout << "FS load failed" << std::endl;
			return false;
		}

		if (!CompileShader(VertexShader, VertexShaderCode)) {
			std::cout << "VS compile failed" << std::endl;
			return false;
		}

		if (!CompileShader(FragmentShader, FragmentShaderCode)) {
			std::cout << "FS compile failed" << std::endl;
			return false;
		}

		if (!LinkVertexAndFragmentShader(ProgramID, VertexShader, FragmentShader))
			return false;

		return true;
	}

	bool BaseRender::CheckCompilationStatus(GLuint ShaderID) const noexcept
	{
		GLint ret;
		glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &ret);

		return ret;
	}

	bool BaseRender::CheckLinkageStatus(GLuint ProgramID) const noexcept
	{
		GLint ret;
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &ret);

		return ret;
	}

	bool BaseRender::LoadVertexShader(const std::string &vertex_file_path, std::string &vertex_shader_code) const
	{
		std::string VertexShaderCode;
		std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);

		if (VertexShaderStream.is_open())
		{
			std::string Line = "";
			while (getline(VertexShaderStream, Line))
				VertexShaderCode += "\n" + Line;
			VertexShaderStream.close();
		}
		else
			return GL_FALSE;

		vertex_shader_code = std::move(VertexShaderCode);

		return GL_TRUE;
	}

	bool BaseRender::LoadFragmentShader(const std::string &fragment_file_path, std::string &fragment_shader_code) const
	{
		std::string FragmentShaderCode;
		std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);

		if (FragmentShaderStream.is_open())
		{
			std::string line = "";
			while (getline(FragmentShaderStream, line))
				FragmentShaderCode += "\n" + line;
			FragmentShaderStream.close();
		}
		else
		{
			std::cout << "can not open fragment shader file\n" << std::endl;
			return GL_FALSE;
		}

		fragment_shader_code = std::move(FragmentShaderCode);

		return GL_TRUE;
	}

	bool BaseRender::CompileShader(GLuint ShaderID, const std::string &SourceCode) const
	{
		const char *VertexSourcePointer = SourceCode.c_str();
		glShaderSource(ShaderID, 1, &VertexSourcePointer, nullptr);
		glCompileShader(ShaderID);

		if (CheckCompilationStatus(ShaderID) != GL_TRUE)
			return GL_FALSE;
		else
			return GL_TRUE;
	}

	void BaseRender::CreateLightInfo(const glm::vec3 &pos, const glm::vec3 &dir,LightType type,InteractionType interactive)
	{
		pLightInfo = std::make_shared<BaseLighting>(pos,dir,type,interactive);
	}

	bool BaseRender::LinkVertexAndFragmentShader(GLuint ProgramID, GLuint VertexShader, GLuint FragmentShader) const
	{
		glAttachShader(ProgramID, VertexShader);
		glAttachShader(ProgramID, FragmentShader);
		glLinkProgram(ProgramID);

		if (!CheckLinkageStatus(ProgramID))
			return false;
		else
			return true;
	}


	void BaseRender::ActiveShader()
	{
		if (ProgramID)
			glUseProgram(ProgramID);
	}



	RendererManager::RendererManager()
	{
	}

	RendererManager::~RendererManager()
	{

	}

	ShadowMapRender::ShadowMapRender(GLuint Width, GLuint Height,
		const std::string &vertex_file_path, const std::string &fragment_file_path)
		:m_FBO(0),depthTexture(0),BaseRender(vertex_file_path,fragment_file_path),
		WindowWidth(Width),WindowHeight(Height), ShadowCubeMap(0),
		depthProjectionMatrix(1.0f),depthViewMatrix(1.0f), depthVP(1.0f)
	{
		CubeMapIterator[0] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3(1.0f,0.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f) };
		CubeMapIterator[1] = { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.0f,0.0f,0.0f), glm::vec3(0.0f,-1.0f,0.0f) };
		CubeMapIterator[2] = { GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3(0.0f,1.0f,0.0f), glm::vec3(0.0f,0.0f,-1.0f) };
		CubeMapIterator[3] = { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3(0.0f,-1.0f,0.0f), glm::vec3(0.0f,0.0f,-1.0f) };
		CubeMapIterator[4] = { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3(0.0f,0.0f,1.0f), glm::vec3(0.0f,-1.0f,0.0f) };
		CubeMapIterator[5] = { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3(0.0f,0.0f,-1.0f), glm::vec3(0.0f,-1.0f,0.0f) };
	}

	ShadowMapRender::~ShadowMapRender()
	{
	
	}

	void ShadowMapRender::SetTransformationMatrix(const glm::mat4 &Projection, const glm::mat4 &View) noexcept
	{
		depthProjectionMatrix = Projection;
		depthViewMatrix = View;

		depthVP = Projection * View;
	}

	void BaseRender::SetLightPos(const glm::vec3 &pos) noexcept
	{
		if (pLightInfo)
			pLightInfo->SetLightPos(pos);
	}

	ShadowMapRender::CubeMapCameraPose* ShadowMapRender::GetCameraPose() noexcept
	{
		return CubeMapIterator;
	}

	glm::vec3 BaseRender::GetLightPos() const noexcept
	{
		if (pLightInfo)
			return pLightInfo->GetLightPos();
		else
			return glm::vec3(0,0,0);
	}

	GLuint ShadowMapRender::GetFBO() const noexcept
	{
		return m_FBO;
	}

	GLuint ShadowMapRender::GetWidth() const noexcept
	{
		return WindowWidth;
	}

	GLuint ShadowMapRender::GetHeight() const noexcept
	{
		return WindowHeight;
	}

	glm::mat4 ShadowMapRender::GetDepthVP() const noexcept
	{
		return depthVP;
	}

	GLuint ShadowMapRender::GetDepthTexture() const noexcept
	{
		return depthTexture;
	}

	void BaseRender::SetLightType(LightType type) noexcept
	{
		if (pLightInfo)
			pLightInfo->SetLightType(type);
	}


	LightType BaseRender::GetLightType() const noexcept
	{
		if (pLightInfo)
			return pLightInfo->GetLightType();

		return LightType::InvalidType;
	}

	GLuint ShadowMapRender::GetShadowCubeMap() const noexcept
	{
		return ShadowCubeMap;
	}

	bool ShadowMapRender::InitShadowMapRender() noexcept
	{
		// Create framebuffer object for rendering
		glGenFramebuffers(1, &m_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

		if (!pLightInfo)
			return false;

		auto Type = pLightInfo->GetLightType();

		if (Type == LightType::Directional ||
			Type == LightType::Spot)
		{
			// Two pass shadow map
			if (Type == LightType::Directional) {
				// Parallel light
				depthProjectionMatrix = glm::ortho<float>(-100, 100, -100, 100, -100, 200);
				depthViewMatrix = glm::lookAt(pLightInfo->GetLightPos(), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
				depthVP = depthProjectionMatrix * depthViewMatrix;
			}
			else
			{
				// Spot Light
				depthProjectionMatrix = glm::perspective(glm::radians(90.0f), 4.0f / 3.0f, 0.1f, 1000.0f);
				depthViewMatrix = glm::lookAt(pLightInfo->GetLightPos(),pLightInfo->GetLightPos() + pLightInfo->GetDirection(),glm::vec3(0,-1,0));
				depthVP = depthProjectionMatrix * depthViewMatrix;
			}

			// Depth texture
			glGenTextures(1, &depthTexture);
			glBindTexture(GL_TEXTURE_2D, depthTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, WindowWidth,
				WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

			// Attach the depth texture to the depth attachment point
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

			// Disable the color output and input
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
		else if (Type == LightType::Omni_Directional)
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
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0,GL_DEPTH_COMPONENT32, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
			}

			glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, ShadowCubeMap, 0);

			glDrawBuffer(GL_NONE);
		}

		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (Status != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("Framebuffer error, status: 0x%x\n", Status);

			if (m_FBO)
				glDeleteFramebuffers(1, &m_FBO);
			if (depthTexture)
				glDeleteTextures(1, &depthTexture);

			return false;
		}
		
		return true;
	}

	BaseRender* RendererManager::GetRenderPtr(const std::string &name) noexcept
	{
		auto it = m_Renders.find(name);
		if (it != m_Renders.end())
			return it->second;
		else
			return nullptr;
	}

	GLint RendererManager::GetRenderProgramID(const std::string &name) const noexcept
	{
		auto it = m_Renders.find(name);
		if (it != m_Renders.end())
			return it->second->GetProgramID();
		else
			return -1;
	}

	void RendererManager::addRender(const std::string &name, BaseRender * pRender) noexcept
	{
		m_Renders.insert(std::make_pair(name,pRender));
	}

	void RendererManager::deleteRender(const std::string &name) noexcept
	{
		auto it = m_Renders.find(name);
		if (it != m_Renders.end())
			m_Renders.erase(it);
	}

}