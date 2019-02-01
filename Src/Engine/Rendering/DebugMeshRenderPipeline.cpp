#include "Rendering/DebugMeshRenderPipeline.h"
#include "Rendering/RendererContext.h"
#include "Geometry/DebugMesh.h"
#include "World/World.h"

namespace cxc
{
	DebugMeshRenderPipeline::DebugMeshRenderPipeline():
		ForwardRenderPipeline("DebugMeshRenderPipeline")
	{

	}

	DebugMeshRenderPipeline::~DebugMeshRenderPipeline()
	{

	}

	void DebugMeshRenderPipeline::Render(std::shared_ptr<RendererContext> Context)
	{
		auto pWorld = World::GetInstance();
		auto BindedSubMeshes = Context->GetBindedSubMeshes();
		for (auto pSubMesh : BindedSubMeshes)
		{
			auto OwnerMesh = pSubMesh->GetOwnerMesh();
			auto pDebugMesh = std::dynamic_pointer_cast<DebugMesh>(OwnerMesh);
			if (!pDebugMesh)
				continue;

			auto pOwnerMesh = pSubMesh->GetOwnerMesh();
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, pWorld->pWindowMgr->GetWindowWidth(), pWorld->pWindowMgr->GetWindowHeight());

			GLuint VertexColorLoc = glGetUniformLocation(ProgramID, "VertexColor");
			glm::vec3 Color = pDebugMesh->GetDebugMeshColor();
			glUniform3f(VertexColorLoc, Color.x, Color.y, Color.z);

			GLuint M_MatrixID = glGetUniformLocation(ProgramID, "M");
			// Set model matrix																										  
			glUniformMatrix4fv(M_MatrixID, 1, GL_FALSE, &pOwnerMesh->GetModelMatrix()[0][0]);

			// Draw the mesh
			pSubMesh->DrawSubMesh();
		}
	}
}