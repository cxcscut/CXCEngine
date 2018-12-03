#include "ForwardRender.h"

#if WIN32

#include "..\Scene\Mesh.h"
#include "..\World\World.h"

#else

#include "../Scene/Mesh.h"
#include "../World/World.h"

#endif

namespace cxc
{
	ForwardRender::ForwardRender()
	{

	}

	ForwardRender::ForwardRender(const std::string& Name):
		MeshRender(Name)
	{

	}

	ForwardRender::~ForwardRender()
	{

	}

	bool ForwardRender::InitializeRender()
	{
		bool bSuccessful = true;

		// Initialize all the pipelines
		bSuccessful &= pFowardRenderPipeline->InitializePipeline();

		return bSuccessful;
	}

	void ForwardRender::Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights)
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
		BindLightUniforms(Lights);

		M_MatrixID = glGetUniformLocation(ProgramID, "M");
		Eyepos_loc = glGetUniformLocation(ProgramID, "EyePosition_worldspace");

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
		pMesh->BindMaterial(ProgramID);

		// Draw the mesh
		pMesh->DrawMesh();
	}

	void ForwardRender::SetForwardRenderPipeline(std::shared_ptr<ForwardRenderPipeline> Pipeline)
	{
		if (Pipeline)
		{
			pFowardRenderPipeline = Pipeline;
			Pipeline->SetOwnerRender(shared_from_this());
		}
	}
}