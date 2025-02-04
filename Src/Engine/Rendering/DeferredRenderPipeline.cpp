#include "Rendering/DeferredRenderPipeline.h"
#include "Rendering/DeferredRenderer.h"
#include "Rendering/RendererContext.h"
#include "Geometry/SubMesh.h"
#include "World/World.h"

namespace cxc
{
	static const GLfloat LightingPassVertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f
	};

	static const GLfloat LightingPassTexCoords[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	static const uint32_t LightingPassIndices[] = {
		0, 1, 2,
		2, 3, 0
	};

	DeferredRenderPipeline::DeferredRenderPipeline():
		SubMeshRenderPipeline("DeferredRenderPipeline"),
		SceenQuardVAO(0), SceenQuardEBO(0),
		SceenQuardVerticesVBO(0), SceenQuardTexCoordsVBO(0)
	{
		CreateSceenQuardBuffers();
	}

	DeferredRenderPipeline::~DeferredRenderPipeline()
	{
		ReleaseSceenQuardBuffers();
	}
	
	void DeferredRenderPipeline::Render(std::shared_ptr<RendererContext> Context)
	{
		auto pSceneManager = SceneManager::GetInstance();
		auto Lights = Context->GetLightsArray();

		// Geometry pass
		GeometryPass(Context, Lights);

		// Lighting pass
		LightingPass(Context, Lights);
	}

	void DeferredRenderPipeline::GeometryPass(std::shared_ptr<RendererContext> Context, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{
		// Geometry pass
		auto pWorld = World::GetInstance();
		auto pWindowMgr = pWorld->pWindowMgr;
		auto pDeferredRenderer = std::dynamic_pointer_cast<DeferredRenderer>(pOwnerRenderer.lock());
		if (!pDeferredRenderer)
			return;

		auto BindedSubMeshed = Context->GetBindedSubMeshes();
		for (auto pSubMesh : BindedSubMeshed)
		{
			auto pOwnerMesh = pSubMesh->GetOwnerMesh();
			if (!pDeferredRenderer)
				return;

			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glCullFace(GL_BACK);
			glBindFramebuffer(GL_FRAMEBUFFER, pDeferredRenderer->GetGBufferID());
			glViewport(0, 0, pWindowMgr->GetWindowWidth(), pWindowMgr->GetWindowHeight());

			// Active the geometry pass
			GLsizei ActiveSubroutinesUniformCountVS, ActiveSubroutinesUniformCountFS;
			glGetProgramStageiv(ProgramID, GL_VERTEX_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &ActiveSubroutinesUniformCountVS);
			glGetProgramStageiv(ProgramID, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &ActiveSubroutinesUniformCountFS);
			std::vector<GLuint> SubroutineIndicesVS(ActiveSubroutinesUniformCountVS, 0);
			std::vector<GLuint> SubroutineIndicesFS(ActiveSubroutinesUniformCountFS, 0);

			GLint RenderPassSelectionVSLoc = glGetSubroutineUniformLocation(ProgramID, GL_VERTEX_SHADER, "RenderPassSelectionVS");
			GLint RenderPassSelectionFSLoc = glGetSubroutineUniformLocation(ProgramID, GL_FRAGMENT_SHADER, "RenderPassSelectionFS");
			GLuint DeferredRenderingGeometryPassVSIndex = glGetSubroutineIndex(ProgramID, GL_VERTEX_SHADER, "GeometryPassVS");
			GLuint DeferredRenderingGeometryPassFSIndex = glGetSubroutineIndex(ProgramID, GL_FRAGMENT_SHADER, "GeometryPassFS");

			if (RenderPassSelectionVSLoc >= 0)
				SubroutineIndicesVS[RenderPassSelectionVSLoc] = DeferredRenderingGeometryPassVSIndex;

			if (RenderPassSelectionFSLoc >= 0)
				SubroutineIndicesFS[RenderPassSelectionFSLoc] = DeferredRenderingGeometryPassFSIndex;

			// Bind the lights uniforms
			BindLightUniforms(Lights, SubroutineIndicesFS);

			// Set model matrix	
			GLuint M_MatrixID = glGetUniformLocation(ProgramID, "M");
			glUniformMatrix4fv(M_MatrixID, 1, GL_FALSE, &pOwnerMesh->GetModelMatrix()[0][0]);

			// Bind the material of the mesh
			MaterialDiffuseSubroutineInfo DiffuseModelInfo;
			DiffuseModelInfo.SubroutineUniformName = "DiffuseModelSelection";
			DiffuseModelInfo.TexturedSubroutineName = "TextureDiffuse";
			DiffuseModelInfo.NonTexturedSubroutineName = "NonTextureDiffuse";
			pSubMesh->BindMaterial(ProgramID, DiffuseModelInfo, SubroutineIndicesFS);

			// Subroutines selections array
			if (ActiveSubroutinesUniformCountVS > 0)
				glUniformSubroutinesuiv(GL_VERTEX_SHADER, ActiveSubroutinesUniformCountVS, &SubroutineIndicesVS.front());

			if (ActiveSubroutinesUniformCountFS > 0)
				glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, ActiveSubroutinesUniformCountFS, &SubroutineIndicesFS.front());

			// Draw the mesh
			pSubMesh->DrawSubMesh();
		}
	}

	void DeferredRenderPipeline::LightingPass(std::shared_ptr<RendererContext> Context, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{
		// In the lighting pass of deferred rendering we should do:
		// 1. Bind the framebuffer object to the default framebuffer
		// 2. Disable depth test and clear the color buffer
		// 3. Render a quard covering the whole sceen, and the texcoords of the quard ranging between 0.0f and 1.0f
		auto pWorld = World::GetInstance();
		auto pDeferredRenderer = std::dynamic_pointer_cast<DeferredRenderer>(pOwnerRenderer.lock());
		assert(pDeferredRenderer != nullptr);
		if (!pDeferredRenderer)
			return;

		auto BindedSubMeshes = Context->GetBindedSubMeshes();
		for (auto pSubMesh : BindedSubMeshes)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glClear(GL_COLOR_BUFFER_BIT);
			glViewport(0, 0, pWorld->pWindowMgr->GetWindowWidth(), pWorld->pWindowMgr->GetWindowHeight());

			// Active the lighting pass
			GLsizei ActiveSubroutinesUniformCountVS, ActiveSubroutinesUniformCountFS;
			glGetProgramStageiv(ProgramID, GL_VERTEX_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &ActiveSubroutinesUniformCountVS);
			glGetProgramStageiv(ProgramID, GL_FRAGMENT_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &ActiveSubroutinesUniformCountFS);
			std::vector<GLuint> SubroutineIndicesVS(ActiveSubroutinesUniformCountVS, 0);
			std::vector<GLuint> SubroutineIndicesFS(ActiveSubroutinesUniformCountFS, 0);

			GLint RenderPassSelectionVSLoc = glGetSubroutineUniformLocation(ProgramID, GL_VERTEX_SHADER, "RenderPassSelectionVS");
			GLint RenderPassSelectionFSLoc = glGetSubroutineUniformLocation(ProgramID, GL_FRAGMENT_SHADER, "RenderPassSelectionFS");
			GLuint DeferredRenderingLightingPassVSIndex = glGetSubroutineIndex(ProgramID, GL_VERTEX_SHADER, "LightingPassVS");
			GLuint DeferredRenderingLightingPassFSIndex = glGetSubroutineIndex(ProgramID, GL_FRAGMENT_SHADER, "LightingPassFS");

			if (RenderPassSelectionVSLoc >= 0)
			{
				SubroutineIndicesVS[RenderPassSelectionVSLoc] = DeferredRenderingLightingPassVSIndex;
			}

			if (RenderPassSelectionFSLoc >= 0)
			{
				SubroutineIndicesFS[RenderPassSelectionFSLoc] = DeferredRenderingLightingPassFSIndex;
			}

			// Bind the material of the mesh
			MaterialDiffuseSubroutineInfo DiffuseModelInfo;
			DiffuseModelInfo.SubroutineUniformName = "DiffuseModelSelection";
			DiffuseModelInfo.TexturedSubroutineName = "TextureDiffuse";
			DiffuseModelInfo.NonTexturedSubroutineName = "NonTextureDiffuse";
			pSubMesh->BindMaterial(ProgramID, DiffuseModelInfo, SubroutineIndicesFS);

			// Bind the lights uniforms
			BindLightUniforms(Lights, SubroutineIndicesFS);

			// Bind G-Buffer textures, the texture unit 0 and 1 are reserved
			GLuint VertexPositionTexLoc = glGetUniformLocation(ProgramID, "VertexPositionTex");
			GLuint VertexDiffuseTexLoc = glGetUniformLocation(ProgramID, "VertexDiffuseTex");
			GLuint VertexNormalTexLoc = glGetUniformLocation(ProgramID, "VertexNormalTex");
			glUniform1i(VertexPositionTexLoc, 2);
			glUniform1i(VertexDiffuseTexLoc, 3);
			glUniform1i(VertexNormalTexLoc, 4);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, pDeferredRenderer->GetVertexPositionTextureID());
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, pDeferredRenderer->GetVertexDiffuseTextureID());
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, pDeferredRenderer->GetVertexNormalTextureID());

			GLuint Eyepos_loc = glGetUniformLocation(ProgramID, "EyePosition");
			auto CurrentActiveCamera = pWorld->pSceneMgr->GetCurrentActiveCamera();
			if (CurrentActiveCamera)
			{
				glm::vec3 EyePosition = CurrentActiveCamera->EyePosition;
				glUniform3f(Eyepos_loc, EyePosition.x, EyePosition.y, EyePosition.z);
			}

			if (ActiveSubroutinesUniformCountVS > 0)
				glUniformSubroutinesuiv(GL_VERTEX_SHADER, ActiveSubroutinesUniformCountVS, &SubroutineIndicesVS.front());

			if (ActiveSubroutinesUniformCountFS > 0)
				glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, ActiveSubroutinesUniformCountFS, &SubroutineIndicesFS.front());

			// Draw sceen quard
			DrawSceenQuard();
		}
	}

	void DeferredRenderPipeline::DrawSceenQuard()
	{
		glBindVertexArray(SceenQuardVAO);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, SceenQuardVerticesVBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, SceenQuardTexCoordsVBO);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SceenQuardEBO);

		glDisable(GL_DEPTH_TEST);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
		glEnable(GL_DEPTH_TEST);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}

	void DeferredRenderPipeline::CreateSceenQuardBuffers()
	{
		glGenVertexArrays(1, &SceenQuardVAO);
		glBindVertexArray(SceenQuardVAO);

		glGenBuffers(1, &SceenQuardEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SceenQuardEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(LightingPassIndices), LightingPassIndices, GL_STATIC_DRAW);

		glGenBuffers(1, &SceenQuardVerticesVBO);
		glBindBuffer(GL_ARRAY_BUFFER, SceenQuardVerticesVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(LightingPassVertices), LightingPassVertices, GL_STATIC_DRAW);

		glGenBuffers(1, &SceenQuardTexCoordsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, SceenQuardTexCoordsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(LightingPassTexCoords), LightingPassTexCoords, GL_STATIC_DRAW);
	}

	void DeferredRenderPipeline::ReleaseSceenQuardBuffers()
	{
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		
		// Release buffers
		if (glIsVertexArray(SceenQuardVAO))
			glDeleteVertexArrays(1, &SceenQuardVAO);
		
		if (glIsBuffer(SceenQuardEBO))
			glDeleteBuffers(1, &SceenQuardEBO);

		if (glIsBuffer(SceenQuardVerticesVBO))
			glDeleteBuffers(1, &SceenQuardVerticesVBO);

		if (glIsBuffer(SceenQuardTexCoordsVBO))
			glDeleteBuffers(1, &SceenQuardTexCoordsVBO);
	}
}