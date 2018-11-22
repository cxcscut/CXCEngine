#include "Render.h"

namespace cxc
{
	Render::Render():
		bIsRenderActive(false)
	{

	}

	Render::Render(const std::string& Name)
		: Render()
	{
		RenderName = Name;
	}

	Render::~Render()
	{

	}

	bool Render::InitializeRender()
	{
		bool bSuccessful = true;
		// Initialize all the pipelines
		for (auto pPipeline : pRenderingPipelines)
		{
			if (pPipeline)
				bSuccessful &= pPipeline->InitializePipeline();
		}

		return bSuccessful;
	}

	void Render::UsePipeline(PipelineType Type)
	{
		for (uint16_t i = 0; i < pRenderingPipelines.size(); ++i)
		{
			if (pRenderingPipelines[i] && pRenderingPipelines[i]->GetPipelineType() == Type)
			{
				pRenderingPipelines[i]->UsePipeline();
				break;
			}
		}
	}

	void Render::AddRenderingPipeline(std::shared_ptr<RenderingPipeline> pPipeline)
	{
		pPipeline->SetOwnerRender(shared_from_this());

		pRenderingPipelines.push_back(pPipeline);
	}

	std::shared_ptr<RenderingPipeline> Render::GetPipelinePtr(PipelineType Type)
	{
		for (auto pPipeline : pRenderingPipelines)
		{
			if (pPipeline && pPipeline->GetPipelineType() == Type)
			{
				return pPipeline;
			}
		}

		return nullptr;
	}

}