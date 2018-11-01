#include "MaterialManager.h"

#ifdef WIN32

#include "..\Graphics\RendererManager.h"

#else

#include "../Graphics/RendererManager.h"

#endif // WIN32

namespace cxc {

	MaterialManager::MaterialManager()
	{

	}

	MaterialManager::~MaterialManager()
	{
		CachedMaterials.clear();
	}

	void MaterialManager::addMaterial(std::shared_ptr<Material> pMaterial)
	{;
		CachedMaterials.insert(std::make_pair(pMaterial->MaterialName, pMaterial));
	}
}
