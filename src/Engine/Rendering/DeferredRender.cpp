#include "DeferredRender.h"

#if WIN32

#include "..\Window\Window.h"

#else 

#include "../Window/Window.h"

#endif

namespace cxc
{
	DeferredRender::DeferredRender():
		GeometryFrameBuffer(0), VertexPositionTexture(0),
		VertexColorTexture(0), VertexNormalTexture(0)
	{

	}

	DeferredRender::~DeferredRender()
	{

	}

	void DeferredRender::CreateGBufferTextures()
	{
		auto pWindowMgr = WindowManager::GetInstance();

		// Create vertex position texture
		glGenTextures(1, &VertexPositionTexture);
		glBindTexture(GL_TEXTURE_2D, VertexPositionTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, pWindowMgr->GetWindowWidth(), pWindowMgr->GetWindowHeight(), 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, VertexPositionTexture, 0);

		// Create vertex color texture
		glGenTextures(1, &VertexColorTexture);
		glBindTexture(GL_TEXTURE_2D, VertexColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, pWindowMgr->GetWindowWidth(), pWindowMgr->GetWindowHeight(), 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, VertexColorTexture, 0);

		// Create vertex normal texture
		glGenTextures(1, &VertexNormalTexture);
		glBindTexture(GL_TEXTURE_2D, VertexNormalTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, pWindowMgr->GetWindowWidth(), pWindowMgr->GetWindowHeight(), 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, VertexNormalTexture, 0);

		// Specify the color attachments that will be used
		GLuint Attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, Attachments);
	}
}