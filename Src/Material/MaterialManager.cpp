#include "Material/MaterialManager.h"
#include "Rendering/RendererManager.h"

namespace cxc {

	MaterialManager::MaterialManager()
	{
		auto DefaultMaterial = std::make_shared<Material>();

		// Create default material in the first place
		addMaterial(DefaultMaterial);
	}

	MaterialManager::~MaterialManager()
	{
		CachedMaterials.clear();
	}

	void MaterialManager::addMaterial(std::shared_ptr<Material> pMaterial)
	{
		CachedMaterials.insert(std::make_pair(pMaterial->MaterialName, pMaterial));
	}

	std::shared_ptr<Material> MaterialManager::GetMaterial(const std::string& MaterialName)
	{
		auto iter = CachedMaterials.find(MaterialName);
		if (iter != CachedMaterials.end())
		{
			return iter->second;
		}
		else
			return nullptr;
	}

	std::shared_ptr<Material> MaterialManager::GetMaterial(std::shared_ptr<Material> pMaterial)
	{
		if (pMaterial)
			return GetMaterial(pMaterial->MaterialName);
		else
			return nullptr;
	}
}
