#include "MeshRender.h"

#ifndef CXC_FORWARDRENDER_H
#define CXC_FORWARDRENDER_H

namespace cxc
{
	class ForwardRenderPipeline;

	class ForwardRender : public MeshRender
	{
	public:

		ForwardRender();
		ForwardRender(const std::string& Name);
		virtual ~ForwardRender();

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