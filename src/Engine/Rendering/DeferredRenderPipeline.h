#include "MeshRender.h"

#ifndef CXC_DEFERREDRENDERPIPELINE_H
#define CXC_DEFERREDRENDERPIPELINE_H

namespace cxc
{
	class DeferredRender;

	class DeferredRenderPipeline : public MeshRenderPipeline
	{
	public:

		DeferredRenderPipeline();
		virtual ~DeferredRenderPipeline();

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) override;
		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) override;
		virtual void PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) override;

	private:

		void DrawSceenQuard();
		void CreateSceenQuardBuffers();
		void ReleaseSceenQuardBuffers();

		bool bShouldDrawSceenQuard;

		GLuint SceenQuardVAO, SceenQuardEBO, SceenQuardVerticesVBO, SceenQuardTexCoordsVBO;
	};
}

#endif // CXC_DEFERREDRENDERPIPELINE_H