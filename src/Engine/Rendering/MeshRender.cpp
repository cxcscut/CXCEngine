#include "MeshRender.h"

#ifdef WIN32

#include "..\Scene\SceneManager.h"
#include "..\World\World.h"

#else

#include "../Scene/SceneManager.h"
#include "../World/World.h"

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

	void MeshRender::UsePipeline(std::shared_ptr<MeshRenderPipeline> Pipeline)
	{
		if (Pipeline)
		{
			glUseProgram(Pipeline->GetPipelineProgramID());
			CurrentUsedPipeline = Pipeline;
		}
	}

	void MeshRender::BindCameraUniforms(GLuint ProgramID)
	{
		auto SceneManager = SceneManager::GetInstance();
		SceneManager->pCamera->BindCameraUniforms(ProgramID);
	}

	MeshRenderPipeline::MeshRenderPipeline()
	{
		ProgramID = glCreateProgram();
		pOwnerRender.reset();
	}

	MeshRenderPipeline::MeshRenderPipeline(const std::string& Name) :
		MeshRenderPipeline()
	{
		PipelineName = Name;
	}

	MeshRenderPipeline::~MeshRenderPipeline()
	{
		if (ProgramID)
			glDeleteProgram(ProgramID);

		pOwnerRender.reset();
	}

	void MeshRenderPipeline::BindLightUniforms(std::vector<std::shared_ptr<LightSource>> Lights)
	{
		GLint LightNumLoc = glGetUniformLocation(ProgramID, "LightNum");
		glUniform1i(LightNumLoc, Lights.size());

		for (size_t LightIndex = 0; LightIndex < Lights.size(); ++LightIndex)
		{
			auto pLight = Lights[LightIndex];
			if (pLight)
			{
				std::string LightUniformNamePrefix = "Lights[" + std::to_string(LightIndex) + "]";
				GLint LightPosLoc = glGetUniformLocation(ProgramID, (LightUniformNamePrefix + ".Position").c_str());
				GLint LightColorLoc = glGetUniformLocation(ProgramID, (LightUniformNamePrefix + ".Color").c_str());
				GLint LightIntensityLoc = glGetUniformLocation(ProgramID, (LightUniformNamePrefix + ".Intensity").c_str());

				// Light properties
				glUniform3f(LightPosLoc, pLight->GetLightPos()[0], pLight->GetLightPos()[1], pLight->GetLightPos()[2]);
				glUniform3f(LightColorLoc, pLight->GetLightColor()[0], pLight->GetLightColor()[1], pLight->GetLightColor()[2]);
				glUniform1f(LightIntensityLoc, pLight->GetIntensity());
			}
		}
	}

	bool MeshRenderPipeline::CheckLinkingStatus(std::string& OutResultLog) const
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

	bool MeshRenderPipeline::InitializePipeline()
	{
		bool bSuccessful = true;

		// Link shaders
		bSuccessful &= LinkShaders();

		return bSuccessful;
	}

	void MeshRenderPipeline::AttachShader(std::shared_ptr<Shader> pShader)
	{
		if (pShader && pShader->IsCompiled())
		{
			// Attach shader and link the program
			glAttachShader(ProgramID, pShader->GetShaderID());

			pShaders.insert(std::make_pair(pShader->GetShaderName(), pShader));
		}
	}

	std::shared_ptr<Shader> MeshRenderPipeline::GetAttachedShader(const std::string& ShaderName)
	{
		auto iter = pShaders.find(ShaderName);
		if (iter != pShaders.end())
		{
			return iter->second;
		}
		else
			return nullptr;
	}

	void MeshRenderPipeline::DetachShader(std::shared_ptr<Shader> pShader)
	{
		if (pShader && pShader->IsCompiled())
		{
			glDetachShader(ProgramID, pShader->GetShaderID());
		}

		auto iter = pShaders.find(pShader->GetShaderName());
		if (iter != pShaders.end())
		{
			pShaders.erase(iter);
		}
	}

	bool MeshRenderPipeline::LinkShaders()
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

}