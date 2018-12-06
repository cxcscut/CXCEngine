#include "DeferredRender.h"

#if WIN32

#include "..\Window\Window.h"
#include "..\Scene\Mesh.h"
#include "..\Rendering\RenderManager.h"
#include "..\World\World.h"

#else 

#include "../Window/Window.h"
#include "../Scene/Mesh.h"
#include "../Rendering/RenderManager.h"
#include "../World/World.h"

#endif

namespace cxc
{
	DeferredRender::DeferredRender():
		GeometryFrameBuffer(0), DepthBuffer(0),
		VertexPositionTexture(0), VertexDiffuseTexture(0), VertexNormalTexture(0),
		bIsGBufferDirty(false)
	{
		RenderName = "DefaultDeferredRender";
	}

	DeferredRender::DeferredRender(const std::string& Name)
		: MeshRender(Name)
	{

	}

	DeferredRender::~DeferredRender()
	{
		if (glIsTexture(VertexDiffuseTexture))
		{
			glDeleteTextures(1, &VertexDiffuseTexture);
		}

		if (glIsTexture(VertexPositionTexture))
		{
			glDeleteTextures(1, &VertexPositionTexture);
		}

		if (glIsTexture(VertexNormalTexture))
		{
			glDeleteTextures(1, &VertexNormalTexture);
		}

		if (glIsRenderbuffer(DepthBuffer))
		{
			glDeleteRenderbuffers(1, &DepthBuffer);
		}

		if (glIsFramebuffer(GeometryFrameBuffer))
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

		// Create framebuffer
		if (!glIsFramebuffer(GeometryFrameBuffer))
		{
			// Create G-Buffer frambuffer object
			glGenFramebuffers(1, &GeometryFrameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, GeometryFrameBuffer);

			// Bind the attachments
			GLuint Attachments[4] = { GL_NONE, GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
			glDrawBuffers(4, Attachments);
		}

		// Create depth buffer
		if (!glIsRenderbuffer(DepthBuffer))
		{
			glGenRenderbuffers(1, &DepthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, DepthBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, pWindowMgr->GetWindowWidth(), pWindowMgr->GetWindowHeight());
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthBuffer);
		}

		if (!glIsTexture(VertexPositionTexture))
		{
			// Create vertex position texture
			glGenTextures(1, &VertexPositionTexture);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, VertexPositionTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, pWindowMgr->GetWindowWidth(), pWindowMgr->GetWindowHeight(), 0, GL_RGB, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, VertexPositionTexture, 0);
		}

		if (!glIsTexture(VertexDiffuseTexture))
		{
			// Create vertex color texture
			glGenTextures(1, &VertexDiffuseTexture);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, VertexDiffuseTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, pWindowMgr->GetWindowWidth(), pWindowMgr->GetWindowHeight(), 0, GL_RGB, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, VertexDiffuseTexture, 0);
		}

		if (!glIsTexture(VertexNormalTexture))
		{
			// Create vertex normal texture
			glGenTextures(1, &VertexNormalTexture);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, VertexNormalTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, pWindowMgr->GetWindowWidth(), pWindowMgr->GetWindowHeight(), 0, GL_RGB, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, VertexNormalTexture, 0);
		}
	}

	void DeferredRender::PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{
		// Clear the G-Buffer for rendering
		if (bIsGBufferDirty && glIsFramebuffer(GeometryFrameBuffer))
		{
			glBindFramebuffer(GL_FRAMEBUFFER, GeometryFrameBuffer);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			bIsGBufferDirty = false;
		}

		UsePipeline(pDeferredRenderPipeline);

		// Geometry pass to rendering the scene to the G-Buffer
		if (pDeferredRenderPipeline)
		{

			pDeferredRenderPipeline->PreRender(pMesh, Lights);
		}
	}

	void DeferredRender::Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{
		UsePipeline(pDeferredRenderPipeline);
		
		// Geometry pass to rendering the scene to the G-Buffer
		if (pDeferredRenderPipeline)
		{
			// Create the G-Buffer and textures;
			CreateGBufferTextures();

			pDeferredRenderPipeline->Render(pMesh, Lights);

			// Marked dirty
			bIsGBufferDirty = true;
		}
	}

	void DeferredRender::PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{
		UsePipeline(pDeferredRenderPipeline);

		// Lighting pass to render the final scene using G-Buffer
		if (pDeferredRenderPipeline)
		{
			BindCameraUniforms(pDeferredRenderPipeline->GetPipelineProgramID());
			pDeferredRenderPipeline->PostRender(pMesh, Lights);
		}
	}
}