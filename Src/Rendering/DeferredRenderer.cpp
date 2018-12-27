#include "Rendering/DeferredRenderer.h"
#include "Window/Window.h"
#include "Geometry/SubMesh.h"
#include "Rendering/RendererManager.h"
#include "World/World.h"

namespace cxc
{
	DeferredRenderer::DeferredRenderer():
		GeometryFrameBuffer(0), DepthBuffer(0),
		VertexPositionTexture(0), VertexDiffuseTexture(0), VertexNormalTexture(0)
	{
		RendererName = "DefaultDeferredRender";
	}

	DeferredRenderer::DeferredRenderer(const std::string& Name)
		: SubMeshRenderer(Name)
	{

	}

	DeferredRenderer::~DeferredRenderer()
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

	bool DeferredRenderer::InitializeRenderer()
	{
		bool bSuccessful = true;

		for (auto Pipeline : RenderPipelines)
		{
			bSuccessful &= Pipeline->InitializePipeline();
		}

		return bSuccessful;
	}

	void DeferredRenderer::CreateGBufferTextures()
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

	void DeferredRenderer::Render(std::shared_ptr<RendererContext> Context, const std::vector<std::shared_ptr<LightSource>>& Lights)
	{
		// Create the G-Buffer and textures if needed;
		CreateGBufferTextures();

		// Clear G-Buffer
		glBindFramebuffer(GL_FRAMEBUFFER, GeometryFrameBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto Pipeline : RenderPipelines)
		{
			UsePipeline(Pipeline);
			BindCameraUniforms(Pipeline->GetPipelineProgramID());
			Pipeline->Render(Context, Lights);
		}
	}
}