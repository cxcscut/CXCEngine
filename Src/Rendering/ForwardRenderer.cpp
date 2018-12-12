#include "Rendering/ForwardRenderer.h"
#include "Scene/Mesh.h"
#include "World/World.h"

namespace cxc
{
	ForwardRenderer::ForwardRenderer()
	{

	}

	ForwardRenderer::ForwardRenderer(const std::string& Name):
		MeshRenderer(Name)
	{

	}

	ForwardRenderer::~ForwardRenderer()
	{

	}

	bool ForwardRenderer::InitializeRender()
	{
		bool bSuccessful = true;

		// Initialize all the pipelines
		bSuccessful &= pFowardRenderPipeline->InitializePipeline();

		return bSuccessful;
	}

	void ForwardRenderer::Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{

		// Use pipeline before submit the uniforms to program
		UsePipeline(pFowardRenderPipeline);
		BindCameraUniforms(pFowardRenderPipeline->GetPipelineProgramID());
		pFowardRenderPipeline->Render(pMesh, Lights);
	}

	ForwardRenderPipeline::ForwardRenderPipeline():
		MeshRenderPipeline()
	{

	}

	ForwardRenderPipeline::ForwardRenderPipeline(const std::string& Name)
		: MeshRenderPipeline(Name)
	{

	}

	ForwardRenderPipeline::~ForwardRenderPipeline()
	{

	}

	void ForwardRenderPipeline::Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{
		GLuint Eyepos_loc, M_MatrixID;

		auto pWorld = World::GetInstance();
		auto pRender = pOwnerRender.lock();
		auto pOwnerObject = pMesh->GetOwnerObject();

		if (Lights.empty())
			return;

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
		glUniformMatrix4fv(M_MatrixID, 1, GL_FALSE, &pOwnerObject->GetObjectModelMatrix()[0][0]);

		// Bind the material of the mesh
		MaterialDiffuseSubroutineInfo DiffuseModelInfo;
		DiffuseModelInfo.SubroutineUniformName = "DiffuseFactorSelection";
		DiffuseModelInfo.TexturedSubroutineName = "TextureDiffuse";
		DiffuseModelInfo.NonTexturedSubroutineName = "NonTextureDiffuse";
		pMesh->BindMaterial(ProgramID, DiffuseModelInfo, SubroutineIndicesFS);

		// Submit the subroutines selections
		if (ActiveSubroutinesUniformCountFS > 0)
		{
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, SubroutineIndicesFS.size(), &SubroutineIndicesFS.front());
		}

		// Draw the mesh
		pMesh->DrawMesh();
	}

	void ForwardRenderer::SetForwardRenderPipeline(std::shared_ptr<ForwardRenderPipeline> Pipeline)
	{
		if (Pipeline)
		{
			pFowardRenderPipeline = Pipeline;
			Pipeline->SetOwnerRender(shared_from_this());
		}
	}
}