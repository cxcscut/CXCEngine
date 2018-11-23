#include "ShaderManager.h"

namespace cxc
{
	ShaderManager::ShaderManager()
	{

	}

	ShaderManager::~ShaderManager()
	{

	}

	std::shared_ptr<Shader> ShaderManager::FactoryShader(const std::string& ShaderName, eShaderType ShaderType, const std::string& ShaderFileName)
	{
		auto pNewShader = NewObject<Shader>(ShaderName, ShaderFileName, ShaderType);
		if (pNewShader && pNewShader->CompileShader())
		{
			AddShader(pNewShader);

			return pNewShader;
		}
		else
			return nullptr;
	}

	std::shared_ptr<Shader>ShaderManager::GetShader(const std::string& ShaderName)
	{
		auto iter = pShadersMap.find(ShaderName);
		if (iter != pShadersMap.end())
		{
			return iter->second;
		}
		else
			return nullptr;
	}

	void ShaderManager::AddShader(std::shared_ptr<Shader> pShader)
	{
		if (pShader)
		{
			pShadersMap.insert(std::make_pair(pShader->GetShaderName(), pShader));
		}
	}

}