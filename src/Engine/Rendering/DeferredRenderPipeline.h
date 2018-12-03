#include "MeshRender.h"

#ifndef CXC_DEFERREDRENDERPIPELINE_H
#define CXC_DEFERREDRENDERPIPELINE_H

namespace cxc
{
	class DeferredRender;

	/* DeferredRenderGeometryPassPipeline is the first passing of the deferred rendering, which renders the scene to obtain G-Buffer */
	class DeferredRenderGeometryPassPipeline : public MeshRenderPipeline
	{
	public:

		DeferredRenderGeometryPassPipeline();
		virtual ~DeferredRenderGeometryPassPipeline();

	public:
		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) override;
	};

	/* DeferredRenderLightingPassPipeline is the second passing of the deferred rendering, which renders the final scene using G-Buffer*/
	class DeferredRenderLightingPassPipeline : public MeshRenderPipeline
	{
	public:

		DeferredRenderLightingPassPipeline();
		virtual ~DeferredRenderLightingPassPipeline();

	public:

		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) override;
	};
}

#endif // CXC_DEFERREDRENDERPIPELINE_H