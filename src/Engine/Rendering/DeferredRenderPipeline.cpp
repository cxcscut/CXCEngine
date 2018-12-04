#include "DeferredRenderPipeline.h"
#include "DeferredRender.h"

#if WIN32

#include "..\Scene\Mesh.h"
#include "..\World\World.h"

#else

#include "../Scene/Mesh.h"
#include "../World/World.h"

#endif

namespace cxc
{
	DeferredRenderPipeline::DeferredRenderPipeline():
		MeshRenderPipeline("DeferredRenderPipeline")
	{

	}

	DeferredRenderPipeline::~DeferredRenderPipeline()
	{

	}

	void DeferredRenderPipeline::PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{
		auto pWorld = World::GetInstance();
		auto pDeferredRender = std::dynamic_pointer_cast<DeferredRender>(pOwnerRender.lock());
		auto pOwnerObject = pMesh->GetOwnerObject();
		assert(pDeferredRender != nullptr);
		if (!pDeferredRender)
			return;

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, pDeferredRender->GetGBufferID());
		glViewport(0, 0, pWorld->pWindowMgr->GetWindowWidth(), pWorld->pWindowMgr->GetWindowHeight());

		// Bind the lights uniforms
		BindLightUniforms(Lights);
		
		// Choose the geometry pass
		GLuint DeferredRenderingGeometryPassFSIndex = glGetSubroutineIndex(ProgramID, GL_FRAGMENT_SHADER, "GeometryPass");
		GLuint DeferredRenderingGeometryPassVSIndex = glGetSubroutineIndex(ProgramID, GL_VERTEX_SHADER, "GeometryPass");
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &DeferredRenderingGeometryPassFSIndex);
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &DeferredRenderingGeometryPassVSIndex);

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
		GLuint M_MatrixID = glGetUniformLocation(ProgramID, "M");
		glUniformMatrix4fv(M_MatrixID, 1, GL_FALSE, &pOwnerObject->getTransMatrix()[0][0]);

		// Bind the material of the mesh
		pMesh->BindMaterial(ProgramID);

		// Draw the mesh
		pMesh->DrawMesh();
	}

	void DeferredRenderPipeline::Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{
		// In the lighting pass of deferred rendering we should do:
		// 1. Bind the framebuffer object to the default framebuffer
		// 2. Disable depth test and clear the color buffer
		// 3. Render a quard covering the whole sceen, and the texcoords of the quard ranging between 0.0f and 1.0f
		auto pWorld = World::GetInstance();
		auto pDeferredRender = std::dynamic_pointer_cast<DeferredRender>(pOwnerRender.lock());
		assert(pDeferredRender != nullptr);
		if (!pDeferredRender)
			return;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, pWorld->pWindowMgr->GetWindowWidth(), pWorld->pWindowMgr->GetWindowHeight());

		// Bind the lights uniforms
		BindLightUniforms(Lights);

		// Active the lighting pass
		GLuint DeferredRenderingLightingPassFSIndex = glGetSubroutineIndex(ProgramID, GL_FRAGMENT_SHADER, "LightingPass");
		GLuint DeferredRenderingLightingPassVSIndex = glGetSubroutineIndex(ProgramID, GL_VERTEX_SHADER, "LightingPass");
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &DeferredRenderingLightingPassFSIndex);
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &DeferredRenderingLightingPassVSIndex);

		// Bind G-Buffer textures
		GLuint VertexPositionTexLoc = glGetUniformLocation(ProgramID, "VertexPositionTex");
		GLuint VertexDiffuseTexLoc = glGetUniformLocation(ProgramID, "VertexDiffuseTex");
		GLuint VertexNormalTexLoc = glGetUniformLocation(ProgramID, "VertexNormalTex");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pDeferredRender->GetVertexPositionTextureID());
		glUniform1i(VertexPositionTexLoc, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pDeferredRender->GetVertexDiffuseTextureID());
		glUniform1i(VertexDiffuseTexLoc, 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, pDeferredRender->GetVertexNormalTextureID());
		glUniform1i(VertexNormalTexLoc, 2);

		GLuint Eyepos_loc = glGetUniformLocation(ProgramID, "EyePosition_worldspace");
		glm::vec3 EyePosition = pWorld->pSceneMgr->pCamera->EyePosition;
		glUniform3f(Eyepos_loc, EyePosition.x, EyePosition.y, EyePosition.z);

		static const GLfloat LightingPassVertices[] = {
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f,
			 1.0f, 1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f
		};

		static const GLfloat LightingPassTexCoords[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			0.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f
		};

		GLuint LigtingPassVerticesVAO, LigtingPassVerticesVBO, LightingPassTexCoordsVBO;
		glGenVertexArrays(1, &LigtingPassVerticesVAO);
		glBindVertexArray(LigtingPassVerticesVAO);

		glGenBuffers(1, &LigtingPassVerticesVBO);
		glBindBuffer(GL_ARRAY_BUFFER, LigtingPassVerticesVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(LightingPassVertices), LightingPassVertices, GL_STATIC_DRAW);

		glGenBuffers(1, &LightingPassTexCoordsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, LightingPassTexCoordsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(LightingPassTexCoords), LightingPassTexCoords, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, LigtingPassVerticesVBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, LightingPassTexCoordsVBO);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnable(GL_DEPTH_TEST);

		// Release buffers
		if (LigtingPassVerticesVAO)
			glDeleteBuffers(1, &LigtingPassVerticesVAO);

		if (LigtingPassVerticesVBO)
			glDeleteBuffers(1, &LigtingPassVerticesVBO);

		if(LightingPassTexCoordsVBO)
			glDeleteBuffers(1, &LightingPassTexCoordsVBO);
	}
}