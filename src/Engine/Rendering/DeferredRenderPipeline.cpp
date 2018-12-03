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
	DeferredRenderGeometryPassPipeline::DeferredRenderGeometryPassPipeline():
		MeshRenderPipeline("GeometryPassPipeline")
	{

	}

	DeferredRenderGeometryPassPipeline::~DeferredRenderGeometryPassPipeline()
	{

	}


	void DeferredRenderGeometryPassPipeline::Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{
		GLint Eyepos_loc, M_MatrixID;

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

	DeferredRenderLightingPassPipeline::DeferredRenderLightingPassPipeline():
		MeshRenderPipeline("LightingPassPipeline")
	{ 

	}

	DeferredRenderLightingPassPipeline::~DeferredRenderLightingPassPipeline()
	{

	}

	void DeferredRenderLightingPassPipeline::Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{

	}

}