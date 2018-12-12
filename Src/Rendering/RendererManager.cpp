#include "Rendering/RendererManager.h"

namespace cxc {

	RendererManager::RendererManager()
	{
		
	}

	RendererManager::~RendererManager()
	{
		
	}

	void RendererManager::SetCurrentUsedRender(std::shared_ptr<MeshRenderer> pRender)
	{
		CurrentUsedRender = pRender;
	}

	std::shared_ptr<Shader> RendererManager::FactoryShader(const std::string& ShaderName, eShaderType ShaderType, const std::string& ShaderFileName)
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

	std::shared_ptr<Shader>RendererManager::GetShader(const std::string& ShaderName)
	{
		auto iter = pShadersMap.find(ShaderName);
		if (iter != pShadersMap.end())
		{
			return iter->second;
		}
		else
			return nullptr;
	}

	void RendererManager::AddShader(std::shared_ptr<Shader> pShader)
	{
		if (pShader)
		{
			pShadersMap.insert(std::make_pair(pShader->GetShaderName(), pShader));
		}
	}

	std::shared_ptr<MeshRenderer> RendererManager::GetRenderPtr(const std::string &name) noexcept
	{
		auto it = RendersMap.find(name);
		if (it != RendersMap.end())
			return it->second;
		else
			return nullptr;

	}

	void RendererManager::UseRender(const std::string& RenderName)
	{
		auto pRender = GetRenderPtr(RenderName);
		if (pRender)
		{
			CurrentUsedRender = pRender;
		}
	}

	void RendererManager::AddRender(std::shared_ptr<MeshRenderer> pRender) noexcept
	{
		if (pRender)
		{
			RendersMap.insert(std::make_pair(pRender->GetRenderName(), pRender));
		}
	}

	void RendererManager::DeleteRender(const std::string &name) noexcept
	{
		auto it = RendersMap.find(name);
		if (it != RendersMap.end())
			RendersMap.erase(it);
	}
}