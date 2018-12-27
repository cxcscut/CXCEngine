#include "SubMeshRenderer.h"

#ifndef CXC_SHADOWRENDERPIPELINE_H
#define CXC_SHADOWRENDERPIPELINE_H

namespace cxc
{
	class SubMesh;
	class LightSource;
	class ShadowRenderer;

	class ShadowRenderLightingPassPipeline : public SubMeshRenderPipeline
	{
	public:

		ShadowRenderLightingPassPipeline();

		virtual ~ShadowRenderLightingPassPipeline();

	public:

		virtual void BindLightUniforms(std::vector<std::shared_ptr<LightSource>> Lights, std::vector<GLuint>& SubroutineIndices) override;

	public:

		virtual void Render(std::shared_ptr<RendererContext> Context, const std::vector<std::shared_ptr<LightSource>>& Lights) override;
	};

	class ShadowRenderBasePassPipeline : public SubMeshRenderPipeline
	{
	public:

		ShadowRenderBasePassPipeline();

		virtual ~ShadowRenderBasePassPipeline();

	public:

		virtual void Render(std::shared_ptr<RendererContext> Context, const std::vector<std::shared_ptr<LightSource>>& Lights) override;

	public:

		void CookShadowMapDepthTexture(std::shared_ptr<RendererContext> Context, const std::vector<std::shared_ptr<LightSource>>& Lights);
		void RenderShadowsToTexture(std::shared_ptr<RendererContext> Context, const std::vector<std::shared_ptr<LightSource>>& Lights);

	};
}

#endif // CXC_SHADOWRENDERPIPELINE_H