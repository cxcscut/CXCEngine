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
		auto pLight = Lights[0];
		auto CameraPos = SceneManager::GetInstance()->pCamera->EyePosition;
		pLight->SetLightPos(CameraPos);
	}

	void MeshRender::Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
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
			auto TexturingPipeline = GetPipelinePtr("TexturingPipeline");
			auto PipelineUsed = CurrentUsedPipeline != TexturingPipeline ? TexturingPipeline : CurrentUsedPipeline;

			// Use pipeline before commit the uniforms to program
			UsePipeline(PipelineUsed);
			BindCameraUniforms(PipelineUsed->GetPipelineProgramID());
			PipelineUsed->Render(pMesh, Lights);
		}
		else
		{
			auto NontexturingPipeline = GetPipelinePtr("NontexturingPipeline");
			auto PipelineUsed = CurrentUsedPipeline != NontexturingPipeline ? NontexturingPipeline : CurrentUsedPipeline;

			// Use pipeline before commit the uniforms to program
			UsePipeline(PipelineUsed);
			BindCameraUniforms(PipelineUsed->GetPipelineProgramID());
			PipelineUsed->Render(pMesh, Lights);
		}
	}

	void MeshRender::PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights)
	{

	}

	void MeshRender::UsePipeline(std::shared_ptr<RenderPipeline> Pipeline)
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

}