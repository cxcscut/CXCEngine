#include "RenderPipeline.h"

#ifndef CXC_SHADOWRENDERPIPELINE_H
#define CXC_SHADOWRENDERPIPELINE_H

namespace cxc
{
	class Mesh;
	class LightSource;
	class ShadowRender;

	class ShadowRenderLightingPassPipeline : public RenderPipeline
	{
	public:

		ShadowRenderLightingPassPipeline();
		ShadowRenderLightingPassPipeline(const std::string& Name);

		virtual ~ShadowRenderLightingPassPipeline();

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) override;
		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) override;
		virtual void PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) override;
	};

	class ShadowRenderBasePassPipeline : public RenderPipeline
	{
	public:

		ShadowRenderBasePassPipeline();

		virtual ~ShadowRenderBasePassPipeline();

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) override;
		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) override;
		virtual void PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) override;

	public:

		void CookShadowMapDepthTexture(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights);
		void RenderShadowsToTexture(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights);

	};
}

#endif // CXC_SHADOWRENDERPIPELINE_H