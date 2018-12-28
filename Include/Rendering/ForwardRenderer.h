#include "SubMeshRenderer.h"

#ifndef CXC_FORWARDRENDER_H
#define CXC_FORWARDRENDER_H

namespace cxc
{
	class ForwardRenderPipeline;

	class CXC_ENGINECORE_API ForwardRenderer : public SubMeshRenderer
	{
	public:

		ForwardRenderer();
		ForwardRenderer(const std::string& Name);
		virtual ~ForwardRenderer();

	public:

		virtual bool InitializeRenderer() override;

	public:

		virtual void Render(std::shared_ptr<RendererContext> Context, const std::vector<std::shared_ptr<LightSource>>& Lights) override;

	};

	class ForwardRenderPipeline : public SubMeshRenderPipeline
	{
	public:

		ForwardRenderPipeline();
		ForwardRenderPipeline(const std::string& Name);

		virtual ~ForwardRenderPipeline();

	public:

		virtual void Render(std::shared_ptr<RendererContext> Context, const std::vector<std::shared_ptr<LightSource>>& Lights) override;
	};
}

#endif // CXC_FORWARDRENDER_H