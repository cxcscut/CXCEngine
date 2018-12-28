#include "Rendering/ForwardRenderer.h"
#include "Rendering/RendererContext.h"
#include "Geometry/SubMesh.h"
#include "World/World.h"

namespace cxc
{
	ForwardRenderer::ForwardRenderer()
	{

	}

	ForwardRenderer::ForwardRenderer(const std::string& Name):
		SubMeshRenderer(Name)
	{

	}

	ForwardRenderer::~ForwardRenderer()
	{

	}

	bool ForwardRenderer::InitializeRenderer()
	{
		bool bSuccessful = true;

		// Initialize all the pipelines
		for (auto Pipeline : RenderingQueue)
		{
			bSuccessful &= Pipeline->InitializePipeline();
		}

		return bSuccessful;
	}

	void ForwardRenderer::Render(std::shared_ptr<RendererContext> Context, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{
		// Use pipeline before submit the uniforms to program
		for (auto Pipeline : RenderingQueue)
		{
			UsePipeline(Pipeline);
			BindCameraUniforms(Pipeline->GetPipelineProgramID());
			Pipeline->Render(Context, Lights);
		}
	}

	ForwardRenderPipeline::ForwardRenderPipeline():
		SubMeshRenderPipeline()
	{

	}

	ForwardRenderPipeline::ForwardRenderPipeline(const std::string& Name)
		: SubMeshRenderPipeline(Name)
	{

	}

	ForwardRenderPipeline::~ForwardRenderPipeline()
	{

	}

	void ForwardRenderPipeline::Render(std::shared_ptr<RendererContext> Context, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{
		if (Lights.empty())
			return;

		GLuint Eyepos_loc, M_MatrixID;

		auto pWorld = World::GetInstance();
		auto BindedSubMeshes = Context->GetBindedSubMeshes();
		for (auto pSubMesh : BindedSubMeshes)
		{
			auto pOwnerMesh = pSubMesh->GetOwnerMesh();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, pWorld->pWindowMgr->GetWindowWidth(), pWorld->pWindowMgr->GetWindowHeight());

			M_MatrixID = glGetUniformLocation(ProgramID, "M");
			Eyepos_loc = glGetUniformLocation(ProgramID, "EyePosition_worldspace");

			// Get subroutine uniforms info
			GLsizei ActiveSubroutinesUniformCountFS;
			glGetProgramStageiv(ProgramID, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &ActiveSubroutinesUniformCountFS);
			std::vector<GLuint> SubroutineIndicesFS(ActiveSubroutinesUniformCountFS, 0);

			auto CurrentActiveCamera = pWorld->pSceneMgr->GetCurrentActiveCamera();
			if (CurrentActiveCamera)
			{
				glm::vec3 EyePosition = CurrentActiveCamera->EyePosition;
				glUniform3f(Eyepos_loc, EyePosition.x, EyePosition.y, EyePosition.z);
			}

			BindLightUniforms(Lights, SubroutineIndicesFS);

			// Set model matrix																										  
			glUniformMatrix4fv(M_MatrixID, 1, GL_FALSE, &pOwnerMesh->GetModelMatrix()[0][0]);

			// Bind the material of the mesh
			MaterialDiffuseSubroutineInfo DiffuseModelInfo;
			DiffuseModelInfo.SubroutineUniformName = "DiffuseFactorSelection";
			DiffuseModelInfo.TexturedSubroutineName = "TextureDiffuse";
			DiffuseModelInfo.NonTexturedSubroutineName = "NonTextureDiffuse";
			pSubMesh->BindMaterial(ProgramID, DiffuseModelInfo, SubroutineIndicesFS);

			// Submit the subroutines selections
			if (ActiveSubroutinesUniformCountFS > 0)
			{
				glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, SubroutineIndicesFS.size(), &SubroutineIndicesFS.front());
			}

			// Draw the mesh
			pSubMesh->DrawSubMesh();
		}
	}
}