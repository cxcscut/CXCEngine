#include "MeshRenderer.h"

#ifndef CXC_FORWARDRENDER_H
#define CXC_FORWARDRENDER_H

namespace cxc
{
	class ForwardRenderPipeline;

	class ForwardRenderer : public MeshRenderer
	{
	public:

		ForwardRenderer();
		ForwardRenderer(const std::string& Name);
		virtual ~ForwardRenderer();

	public:

		virtual bool InitializeRender() override;

	public:

		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) override;

	public:

		void SetForwardRenderPipeline(std::shared_ptr<ForwardRenderPipeline> Pipeline);

	private:

		/* Texturing pipeline */
		std::shared_ptr<ForwardRenderPipeline> pFowardRenderPipeline;

	};

	class ForwardRenderPipeline : public MeshRenderPipeline
	{
	public:

		ForwardRenderPipeline();
		ForwardRenderPipeline(const std::string& Name);

		virtual ~ForwardRenderPipeline();

	public:

		virtual void Render(std::shared_ptr<Mesh> pMesh, const std::vector<std::shared_ptr<LightSource>>& Lights) override;
	};
}

#endif // CXC_FORWARDRENDER_H