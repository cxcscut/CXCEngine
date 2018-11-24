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

	std::shared_ptr<Shader> RenderManager::FactoryShader(const std::string& ShaderName, eShaderType ShaderType, const std::string& ShaderFileName)
	{
		auto pNewShader = NewObject<Shader>(ShaderName, ShaderFileName, ShaderType);
		if (pNewShader && pNewShader->CompileShader())
		{
			pNewShader->ShaderFileName = ShaderFileName;
			AddShader(pNewShader);

			return pNewShader;
		}
		else
			return nullptr;
	}

	std::shared_ptr<Shader>RenderManager::GetShader(const std::string& ShaderName)
	{
		auto iter = pShadersMap.find(ShaderName);
		if (iter != pShadersMap.end())
		{
			return iter->second;
		}
		else
			return nullptr;
	}

	void RenderManager::AddShader(std::shared_ptr<Shader> pShader)
	{
		if (pShader)
		{
			pShadersMap.insert(std::make_pair(pShader->GetShaderName(), pShader));
		}
	}

	std::shared_ptr<MeshRender> RenderManager::GetRenderPtr(const std::string &name) noexcept
	{
		auto it = RendersMap.find(name);
		if (it != RendersMap.end())
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
			RendersMap.insert(std::make_pair(pRender->GetRenderName(), pRender));
		}
	}

	void RenderManager::DeleteRender(const std::string &name) noexcept
	{
		auto it = RendersMap.find(name);
		if (it != RendersMap.end())
			RendersMap.erase(it);
	}
}