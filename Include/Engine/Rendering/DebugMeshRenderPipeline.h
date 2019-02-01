#ifndef CXC_DEBUGMESHRENDERPIPELINE_H
#define CXC_DEBUGMESHRENDERPIPELINE_H

#include "Rendering/ForwardRenderer.h"

namespace cxc
{
	class CXC_ENGINECORE_API DebugMeshRenderPipeline : public ForwardRenderPipeline
	{
	public:
		DebugMeshRenderPipeline();
		virtual ~DebugMeshRenderPipeline();

	public:

		virtual void Render(std::shared_ptr<RendererContext> Context) override;
	};
}

#endif // CXC_DEBUGMESHRENDERPIPELINE_H