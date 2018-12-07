#include "Rendering/MeshRender.h"
#include "Scene/SceneManager.h"
#include "World/World.h"

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
		else
			glUseProgram(0);
	}

	void MeshRender::BindCameraUniforms(GLuint ProgramID)
	{
		auto SceneManager = SceneManager::GetInstance();
		auto CurrentActiveCamera = SceneManager->GetCurrentActiveCamera();
		if(CurrentActiveCamera)
			CurrentActiveCamera->BindCameraUniforms(ProgramID);
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

	void MeshRenderPipeline::BindLightUniforms(std::vector<std::shared_ptr<LightSource>> Lights, std::vector<GLuint>& SubroutineIndices)
	{
		GLint OmniLightNumLoc = glGetUniformLocation(ProgramID, "OmniLightNum");
		GLint DirectionalLightNumLoc = glGetUniformLocation(ProgramID, "DirectionalLightNum");
		
		size_t OmniLightsCount = 0, DirectionalLightsCount = 0;

		for (size_t LightIndex = 0; LightIndex < Lights.size(); ++LightIndex)
		{
			auto pLight = Lights[LightIndex];
			if (pLight)
			{
				GLint LightPosLoc, LightColorLoc, LightIntensityLoc, LightAtteunationLoc;
				GLint LightAtteunationSubroutineIndex;
				std::string LightUniformNamePrefix, LightAtteunationSubroutineName;

				switch (pLight->GetLightType())
				{
				case eLightType::OmniDirectional:
					{
						LightUniformNamePrefix = "OmniLights[" + std::to_string(OmniLightsCount) + "]";
						LightAtteunationSubroutineName = "OmniLightAttenuations[" + std::to_string(OmniLightsCount) + "]";
						OmniLightsCount++;
						break;
					}
				case eLightType::Directional:
					{
						LightUniformNamePrefix = "DirectionalLights[" + std::to_string(DirectionalLightsCount) + "]";
						LightAtteunationSubroutineName = "DirectionalLightAtteunations[" + std::to_string(DirectionalLightsCount) + "]";
						GLuint LightTargetPosLoc = glGetUniformLocation(ProgramID, (LightUniformNamePrefix + ".TargetPos").c_str());
						glUniform3f(LightTargetPosLoc, pLight->GetTargetPos()[0], pLight->GetTargetPos()[1], pLight->GetTargetPos()[2]);
						DirectionalLightsCount++;
						break;
					}
				}

				LightPosLoc = glGetUniformLocation(ProgramID, (LightUniformNamePrefix + ".Position").c_str());
				LightColorLoc = glGetUniformLocation(ProgramID, (LightUniformNamePrefix + ".Color").c_str());
				LightIntensityLoc = glGetUniformLocation(ProgramID, (LightUniformNamePrefix + ".Intensity").c_str());
				LightAtteunationLoc = glGetSubroutineUniformLocation(ProgramID, GL_FRAGMENT_SHADER, (LightAtteunationSubroutineName).c_str());

				switch (pLight->GetAtteunationType())
				{
				case eLightAtteunationType::None:
					LightAtteunationSubroutineIndex = glGetSubroutineIndex(ProgramID, GL_FRAGMENT_SHADER, "None");
					break;

				case eLightAtteunationType::Linear:
					LightAtteunationSubroutineIndex = glGetSubroutineIndex(ProgramID, GL_FRAGMENT_SHADER, "Linear");
					break;

				case eLightAtteunationType::Quadratic:
					LightAtteunationSubroutineIndex = glGetSubroutineIndex(ProgramID, GL_FRAGMENT_SHADER, "Quadratic");
					break;

				case eLightAtteunationType::Cubic:
					LightAtteunationSubroutineIndex = glGetSubroutineIndex(ProgramID, GL_FRAGMENT_SHADER, "Cubic");
					break;
				}
				
				if (LightAtteunationLoc >= 0)
				{
					SubroutineIndices[LightAtteunationLoc] = LightAtteunationSubroutineIndex;
				}

				// Light properties
				glUniform3f(LightPosLoc, pLight->GetLightPos()[0], pLight->GetLightPos()[1], pLight->GetLightPos()[2]);
				glUniform3f(LightColorLoc, pLight->GetLightColor()[0], pLight->GetLightColor()[1], pLight->GetLightColor()[2]);
				glUniform1f(LightIntensityLoc, pLight->GetIntensity());
			}
		}

		glUniform1i(OmniLightNumLoc, OmniLightsCount);
		glUniform1i(DirectionalLightNumLoc, DirectionalLightsCount);
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