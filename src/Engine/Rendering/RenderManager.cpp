#include "RenderManager.h"

namespace cxc {

	RenderManager::RenderManager()
	{
		
	}

	RenderManager::~RenderManager()
	{
		
	}

	void RenderManager::SetCurrentUsedRender(std::shared_ptr<MeshRender> pRender)
	{
		CurrentUsedRender = pRender;
	}

	

	std::shared_ptr<MeshRender> RenderManager::GetRenderPtr(const std::string &name) noexcept
	{
		auto it = m_Renders.find(name);
		if (it != m_Renders.end())
			return it->second;
		else
			return nullptr;

	}

	void RenderManager::UseRender(const std::string& RenderName)
	{
		auto pRender = GetRenderPtr(RenderName);
		if (pRender)
		{
			CurrentUsedRender = pRender;
		}
	}

	void RenderManager::AddRender(std::shared_ptr<MeshRender> pRender) noexcept
	{
		if (pRender)
		{
			m_Renders.insert(std::make_pair(pRender->GetRenderName(), pRender));
		}
	}

	void RenderManager::DeleteRender(const std::string &name) noexcept
	{
		auto it = m_Renders.find(name);
		if (it != m_Renders.end())
			m_Renders.erase(it);
	}

}