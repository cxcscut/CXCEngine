#include "Rendering/RendererManager.h"
#include "Rendering/RendererContext.h"
#include "Geometry/SubMesh.h"

namespace cxc {

	RendererManager::RendererManager()
	{
		
	}

	RendererManager::~RendererManager()
	{
		
	}

	void RendererManager::SetCurrentUsedRenderer(std::shared_ptr<SubMeshRenderer> pRenderer)
	{
		CurrentUsedRenderer = pRenderer;
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

	std::shared_ptr<SubMeshRenderer> RendererManager::GetRendererPtr(const std::string &name) noexcept
	{
		auto it = RenderersMap.find(name);
		if (it != RenderersMap.end())
			return it->second;
		else
			return nullptr;

	}

	void RendererManager::UseRenderer(std::shared_ptr<SubMeshRenderer> pRenderer)
	{
		if (pRenderer)
			CurrentUsedRenderer = pRenderer;
	}

	void RendererManager::UseRenderer(const std::string& RenderName)
	{
		auto pRenderer = GetRendererPtr(RenderName);
		if (pRenderer)
		{
			CurrentUsedRenderer = pRenderer;
		}
	}

	void RendererManager::AddRenderer(std::shared_ptr<SubMeshRenderer> pRenderer) noexcept
	{
		if (pRenderer)
		{
			RenderersMap.insert(std::make_pair(pRenderer->GetRendererName(), pRenderer));
		}
	}

	void RendererManager::DeleteRenderer(const std::string &name) noexcept
	{
		auto it = RenderersMap.find(name);
		if (it != RenderersMap.end())
			RenderersMap.erase(it);
	}

	void RendererManager::AddLightToRendererContext(std::vector<std::shared_ptr<LightSource>> Lights, std::shared_ptr<SubMesh> pSubMesh)
	{
		auto RendererIter = SubMeshRendererBindings.find(pSubMesh);
		if (RendererIter != SubMeshRendererBindings.end())
		{
			auto pRenderer = RendererIter->second;
			auto ContextIter = RendererContextsMap.find(pRenderer->GetRendererName());
			if (ContextIter != RendererContextsMap.end())
			{
				ContextIter->second->SetShadingLights(Lights);
			}
		}
	}

	void RendererManager::AddSubMeshToRendererContext(std::shared_ptr<SubMesh> pSubMesh)
	{
		auto RendererBindingIter = SubMeshRendererBindings.find(pSubMesh);
		if (RendererBindingIter == SubMeshRendererBindings.end())
			return;

		auto pSubMeshRenderer = RendererBindingIter->second;
		auto SubRendererIter = RendererContextsMap.find(pSubMeshRenderer->GetRendererName());
		if (SubRendererIter != RendererContextsMap.end())
		{
			auto Context = SubRendererIter->second;
			Context->AddBindedSubMesh(pSubMesh);
		}
		else
		{
			auto Context = NewObject<RendererContext>(pSubMeshRenderer);
			Context->AddBindedSubMesh(pSubMesh);
			RendererContextsMap.insert(std::make_pair(pSubMeshRenderer->GetRendererName(), Context));
		}
	}

	void RendererManager::ClearRendererContext()
	{
		RendererContextsMap.clear();
	}

	void RendererManager::BindSubMeshRenderer(std::shared_ptr<SubMesh> pSubMesh, std::shared_ptr<SubMeshRenderer> pSubMeshRenderer)
	{
		SubMeshRendererBindings.insert(std::make_pair(pSubMesh, pSubMeshRenderer));
	}

	void RendererManager::UnBindSubMeshRenderer(std::shared_ptr<SubMesh> pSubMesh, std::shared_ptr<SubMeshRenderer> pSubMeshRenderer)
	{
		auto Iter = SubMeshRendererBindings.find(pSubMesh);
		if (Iter != SubMeshRendererBindings.end())
		{
			SubMeshRendererBindings.erase(Iter);
		}
	}
}