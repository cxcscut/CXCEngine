#include "Rendering/SubMeshRenderer.h"
#include "Scene/SceneManager.h"
#include "World/World.h"

namespace cxc
{
	SubMeshRenderer::SubMeshRenderer()
	{

	}

	SubMeshRenderer::SubMeshRenderer(const std::string& Name)
		: SubMeshRenderer()
	{
		RendererName = Name;
	}

	SubMeshRenderer::~SubMeshRenderer()
	{

	}

	void SubMeshRenderer::AddPipeline(std::shared_ptr<SubMeshRenderPipeline> Pipeline)
	{
		Pipeline->SetOwnerRenderer(shared_from_this());
		RenderPipelines.push_back(Pipeline);
	}

	void SubMeshRenderer::UsePipeline(std::shared_ptr<SubMeshRenderPipeline> Pipeline)
	{
		if (Pipeline)
		{
			glUseProgram(Pipeline->GetPipelineProgramID());
			CurrentUsedPipeline = Pipeline;
		}
		else
			glUseProgram(0);
	}

	void SubMeshRenderer::BindCameraUniforms(GLuint ProgramID)
	{
		auto SceneManager = SceneManager::GetInstance();
		auto CurrentActiveCamera = SceneManager->GetCurrentActiveCamera();
		if(CurrentActiveCamera)
			CurrentActiveCamera->BindCameraUniforms(ProgramID);
	}

	SubMeshRenderPipeline::SubMeshRenderPipeline()
	{
		ProgramID = glCreateProgram();
		pOwnerRenderer.reset();
	}

	SubMeshRenderPipeline::SubMeshRenderPipeline(const std::string& Name) :
		SubMeshRenderPipeline()
	{
		PipelineName = Name;
	}

	SubMeshRenderPipeline::~SubMeshRenderPipeline()
	{
		if (ProgramID)
			glDeleteProgram(ProgramID);

		pOwnerRenderer.reset();
	}

	void SubMeshRenderPipeline::BindLightUniforms(std::vector<std::shared_ptr<LightSource>> Lights, std::vector<GLuint>& SubroutineIndices)
	{
		GLint OmniLightNumLoc = glGetUniformLocation(ProgramID, "OmniLightNum");
		GLint DirectionalLightNumLoc = glGetUniformLocation(ProgramID, "DirectionalLightNum");
		GLint SpotLightNumLoc = glGetUniformLocation(ProgramID, "SpotLightNum");
		GLint LightTargetPosLoc, SpotLightCutOffAngleLoc, LightAreaRadiusLoc;
		size_t OmniLightsCount = 0, DirectionalLightsCount = 0, SpotLightCount = 0;

		for (size_t LightIndex = 0; LightIndex < Lights.size(); ++LightIndex)
		{
			auto pLight = Lights[LightIndex];
			if (pLight)
			{
				GLint LightPosLoc, LightColorLoc, LightIntensityLoc, LightAttenuationLoc;
				GLint LightAttenuationSubroutineIndex;
				std::string LightUniformNamePrefix, LightAttenuationSubroutineName;

				switch (pLight->GetLightType())
				{
				case eLightType::OmniDirectional:
					{
						LightUniformNamePrefix = "OmniLights[" + std::to_string(OmniLightsCount) + "]";
						LightAttenuationSubroutineName = "OmniLightAtteunations[" + std::to_string(OmniLightsCount) + "]";
						
						OmniLightsCount++;
						break;
					}
				case eLightType::Directional:
					{
						LightUniformNamePrefix = "DirectionalLights[" + std::to_string(DirectionalLightsCount) + "]";
						LightAttenuationSubroutineName = "DirectionalLightAttenuations[" + std::to_string(DirectionalLightsCount) + "]";
						
						DirectionalLightsCount++;
						break;
					}
				case eLightType::Spot:
					{
						LightUniformNamePrefix = "SpotLights[" + std::to_string(SpotLightCount) + "]";
						LightAttenuationSubroutineName = "SpotLightAttenuations[" + std::to_string(SpotLightCount) + "]";
						SpotLightCount++;
						break;
					}
				}

				SpotLightCutOffAngleLoc = glGetUniformLocation(ProgramID, (LightUniformNamePrefix + ".CutOffAngle").c_str());
				LightTargetPosLoc = glGetUniformLocation(ProgramID, (LightUniformNamePrefix + ".TargetPos").c_str());
				LightPosLoc = glGetUniformLocation(ProgramID, (LightUniformNamePrefix + ".Position").c_str());
				LightColorLoc = glGetUniformLocation(ProgramID, (LightUniformNamePrefix + ".Color").c_str());
				LightIntensityLoc = glGetUniformLocation(ProgramID, (LightUniformNamePrefix + ".Intensity").c_str());
				LightAreaRadiusLoc = glGetUniformLocation(ProgramID, (LightUniformNamePrefix + ".AreaRadius").c_str());
				LightAttenuationLoc = glGetSubroutineUniformLocation(ProgramID, GL_FRAGMENT_SHADER, (LightAttenuationSubroutineName).c_str());

				switch (pLight->GetAtteunationType())
				{
				case eLightAtteunationType::None:
					LightAttenuationSubroutineIndex = glGetSubroutineIndex(ProgramID, GL_FRAGMENT_SHADER, "None");
					break;

				case eLightAtteunationType::Linear:
					LightAttenuationSubroutineIndex = glGetSubroutineIndex(ProgramID, GL_FRAGMENT_SHADER, "Linear");
					break;

				case eLightAtteunationType::Quadratic:
					LightAttenuationSubroutineIndex = glGetSubroutineIndex(ProgramID, GL_FRAGMENT_SHADER, "Quadratic");
					break;

				case eLightAtteunationType::Cubic:
					LightAttenuationSubroutineIndex = glGetSubroutineIndex(ProgramID, GL_FRAGMENT_SHADER, "Cubic");
					break;
				}
				
				if (LightAttenuationLoc >= 0)
				{
					SubroutineIndices[LightAttenuationLoc] = LightAttenuationSubroutineIndex;
				}

				// Light properties
				glUniform1f(SpotLightCutOffAngleLoc, glm::radians(pLight->GetCutOffAngle()));
				glUniform3f(LightTargetPosLoc, pLight->GetTargetPos()[0], pLight->GetTargetPos()[1], pLight->GetTargetPos()[2]);
				glUniform3f(LightPosLoc, pLight->GetLightPos()[0], pLight->GetLightPos()[1], pLight->GetLightPos()[2]);
				glUniform3f(LightColorLoc, pLight->GetLightColor()[0], pLight->GetLightColor()[1], pLight->GetLightColor()[2]);
				glUniform1f(LightIntensityLoc, pLight->GetIntensity());
				glUniform1f(LightAreaRadiusLoc, pLight->GetAreaRadius());
			}
		}

		glUniform1i(OmniLightNumLoc, OmniLightsCount);
		glUniform1i(DirectionalLightNumLoc, DirectionalLightsCount);
		glUniform1i(SpotLightNumLoc, SpotLightCount);
	}

	bool SubMeshRenderPipeline::CheckLinkingStatus(std::string& OutResultLog) const
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

	bool SubMeshRenderPipeline::InitializePipeline()
	{
		bool bSuccessful = true;

		// Link shaders
		bSuccessful &= LinkShaders();

		return bSuccessful;
	}

	void SubMeshRenderPipeline::AttachShader(std::shared_ptr<Shader> pShader)
	{
		if (pShader && pShader->IsCompiled())
		{
			// Attach shader and link the program
			glAttachShader(ProgramID, pShader->GetShaderID());

			pShaders.insert(std::make_pair(pShader->GetShaderName(), pShader));
		}
	}

	std::shared_ptr<Shader> SubMeshRenderPipeline::GetAttachedShader(const std::string& ShaderName)
	{
		auto iter = pShaders.find(ShaderName);
		if (iter != pShaders.end())
		{
			return iter->second;
		}
		else
			return nullptr;
	}

	void SubMeshRenderPipeline::DetachShader(std::shared_ptr<Shader> pShader)
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

	bool SubMeshRenderPipeline::LinkShaders()
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