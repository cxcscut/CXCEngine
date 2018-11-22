#ifdef WIN32

#include "..\General\DefineTypes.h"

#else

#include "../General/DefineTypes.h"

#endif

#ifndef CXC_RENDER_H
#define CXC_RENDER_H

#include "Pipeline.h"

namespace cxc
{
	class Render : public std::enable_shared_from_this<Render>
	{

	public:

		Render();
		Render(const std::string& Name);

		~Render();

	public:

		bool InitializeRender();
		void AddRenderingPipeline(std::shared_ptr<RenderingPipeline> pPipeline);

	public:

		std::string GetRenderName() const { return RenderName; }
		void SetRenderName(const std::string& NewName) { RenderName = NewName; }
		std::shared_ptr<RenderingPipeline> GetPipelinePtr(PipelineType Type);
		void UsePipeline(PipelineType Type);

	private:

		bool bIsRenderActive;

		// Name of the render
		std::string RenderName;

		// Rendering pipeline
		std::vector<std::shared_ptr<RenderingPipeline>> pRenderingPipelines;
	};
}

#endif // CXC_RENDER_H