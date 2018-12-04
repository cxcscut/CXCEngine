#include "DeferredRender.h"

#if WIN32

#include "..\Window\Window.h"
#include "..\Scene\Mesh.h"
#include "..\Rendering\RenderManager.h"

#else 

#include "../Window/Window.h"
#include "../Scene/Mesh.h"
#include "../Rendering/RenderManager.h"

#endif

namespace cxc
{
	DeferredRender::DeferredRender():
		GeometryFrameBuffer(0), VertexPositionTexture(0),
		VertexDiffuseTexture(0), VertexNormalTexture(0)
	{
		RenderName = "DefaultDeferredRender";
	}

	DeferredRender::DeferredRender(const std::string& Name)
		: MeshRender(Name)
	{

	}

	DeferredRender::~DeferredRender()
	{
		if (VertexDiffuseTexture)
		{
			glDeleteTextures(1, &VertexDiffuseTexture);
		}

		if (VertexPositionTexture)
		{
			glDeleteTextures(1, &VertexPositionTexture);
		}

		if (VertexNormalTexture)
		{
			glDeleteTextures(1, &VertexNormalTexture);
		}

		if (GeometryFrameBuffer)
		{
			glDeleteFramebuffers(1, &GeometryFrameBuffer);
		}
	}

	bool DeferredRender::InitializeRender()
	{
		bool bSuccessful = true;

		bSuccessful &= pDeferredRenderPipeline->InitializePipeline();

		return bSuccessful;
	}

	void DeferredRender::SetDeferredRenderPipeline(std::shared_ptr<DeferredRenderPipeline> Pipeline)
	{
		pDeferredRenderPipeline = Pipeline;
		pDeferredRenderPipeline->SetOwnerRender(shared_from_this());
	}

	void DeferredRender::CreateGBufferTextures()
	{
		auto pWindowMgr = WindowManager::GetInstance();

		// Create G-Buffer frambuffer object
		glGenFramebuffers(1, &GeometryFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, GeometryFrameBuffer);

		// Create vertex position texture
		glGenTextures(1, &VertexPositionTexture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, VertexPositionTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, pWindowMgr->GetWindowWidth(), pWindowMgr->GetWindowHeight(), 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, VertexPositionTexture, 0);

		// Create vertex color texture
		glGenTextures(1, &VertexDiffuseTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, VertexDiffuseTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, pWindowMgr->GetWindowWidth(), pWindowMgr->GetWindowHeight(), 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, VertexDiffuseTexture, 0);

		// Create vertex normal texture
		glGenTextures(1, &VertexNormalTexture);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, VertexNormalTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, pWindowMgr->GetWindowWidth(), pWindowMgr->GetWindowHeight(), 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, VertexNormalTexture, 0);

		// Bind the attachments
		GLuint Attachments[4] = {GL_NONE, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(4, Attachments);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void DeferredRender::PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{
		// Create the G-Buffer and textures;
		CreateGBufferTextures();

		// Use pipeline before commit the uniforms to program
		UsePipeline(pDeferredRenderPipeline);
		BindCameraUniforms(pDeferredRenderPipeline->GetPipelineProgramID());
		pDeferredRenderPipeline->PreRender(pMesh, Lights);
	}

	void DeferredRender::Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{
		// Lighting pass to render the final mesh using G-Buffer
		if (pDeferredRenderPipeline)
		{
			UsePipeline(pDeferredRenderPipeline);
			pDeferredRenderPipeline->Render(pMesh, Lights);
		}
	}
}