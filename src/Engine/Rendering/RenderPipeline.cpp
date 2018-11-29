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

	void RenderPipeline::BindLightUniforms(std::vector<std::shared_ptr<BaseLighting>> Lights)
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
			// Attach shader and link the program
			glAttachShader(ProgramID, pShader->GetShaderID());

			pShaders.insert(std::make_pair(pShader->GetShaderName(), pShader));
		}
	}

	std::shared_ptr<Shader> RenderPipeline::GetAttachedShader(const std::string& ShaderName)
	{
		auto iter = pShaders.find(ShaderName);
		if (iter != pShaders.end())
		{
			return iter->second;
		}
		else
			return nullptr;
	}

	void RenderPipeline::DetachShader(std::shared_ptr<Shader> pShader)
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

	void RenderPipeline::PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{
		// Do noting in the base render class
	}

	void RenderPipeline::Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{
		GLint TexSamplerHandle;
		GLint Eyepos_loc, M_MatrixID;
		GLint Ka_loc, Ks_loc, Kd_loc, ShiniessLoc;
		GLint LightPowerLoc;

		auto pWorld = World::GetInstance();
		auto pRender = pOwnerRender.lock();
		auto pOwnerObject = pMesh->GetOwnerObject();

		if (Lights.empty())
			return;

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, pWorld->pWindowMgr->GetWindowWidth(), pWorld->pWindowMgr->GetWindowHeight());
		BindLightUniforms(Lights);

		TexSamplerHandle = glGetUniformLocation(ProgramID, "TexSampler");
		M_MatrixID = glGetUniformLocation(ProgramID, "M");
		Eyepos_loc = glGetUniformLocation(ProgramID, "EyePosition_worldspace");
		Ka_loc = glGetUniformLocation(ProgramID, "Ka");
		Ks_loc = glGetUniformLocation(ProgramID, "Ks");
		Kd_loc = glGetUniformLocation(ProgramID, "Kd");
		ShiniessLoc = glGetUniformLocation(ProgramID, "Shiniess");

		glm::vec3 EyePosition = pWorld->pSceneMgr->pCamera->EyePosition;
		glUniform3f(Eyepos_loc, EyePosition.x, EyePosition.y, EyePosition.z);

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
		pMesh->BindMaterial(Ka_loc, Kd_loc, Ks_loc, ShiniessLoc, TexSamplerHandle);

		// Draw the mesh
		pMesh->DrawMesh();
	}

	void RenderPipeline::PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{
		// Do noting in the base render class
	}
}