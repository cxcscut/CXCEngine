#include "RenderPipeline.h"

#ifndef CXC_SHADOWRENDERPIPELINE_H
#define CXC_SHADOWRENDERPIPELINE_H

namespace cxc
{
	class Mesh;
	class BaseLighting;
	class ShadowMapRender;

	class ShadowedMeshRenderPipeline : public RenderPipeline
	{
	public:

		ShadowedMeshRenderPipeline();
		ShadowedMeshRenderPipeline(const std::string& Name);

		virtual ~ShadowedMeshRenderPipeline();

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights) override;
		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights) override;
		virtual void PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights) override;
	};

	class ShadowMapCookingPipeline : public RenderPipeline
	{
	public:

		ShadowMapCookingPipeline();

		virtual ~ShadowMapCookingPipeline();

	public:

		virtual void PreRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights) override;
		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights) override;
		virtual void PostRender(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights) override;

	public:

		void CookShadowMapDepthTexture(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights);
		void RenderShadowsToTexture(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<BaseLighting>>& Lights);

	};
}

#endif // CXC_SHADOWRENDERPIPELINE_H