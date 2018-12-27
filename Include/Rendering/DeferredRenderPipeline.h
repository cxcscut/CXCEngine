#include "SubMeshRenderer.h"

#ifndef CXC_DEFERREDRENDERPIPELINE_H
#define CXC_DEFERREDRENDERPIPELINE_H

namespace cxc
{
	class DeferredRenderer;

	class DeferredRenderPipeline : public SubMeshRenderPipeline
	{
	public:

		DeferredRenderPipeline();
		virtual ~DeferredRenderPipeline();

	public:

		virtual void Render(std::shared_ptr<RendererContext> Context, const std::vector<std::shared_ptr<LightSource>>& Lights) override;

	private:

		void GeometryPass(std::shared_ptr<RendererContext> Context, const std::vector<std::shared_ptr<LightSource>>& Lights);
		void LightingPass(std::shared_ptr<RendererContext> Context, const std::vector<std::shared_ptr<LightSource>>& Lights);

		void DrawSceenQuard();
		void CreateSceenQuardBuffers();
		void ReleaseSceenQuardBuffers();

		GLuint SceenQuardVAO, SceenQuardEBO, SceenQuardVerticesVBO, SceenQuardTexCoordsVBO;
	};
}

#endif // CXC_DEFERREDRENDERPIPELINE_H