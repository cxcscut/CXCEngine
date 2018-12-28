#include "General/DefineTypes.h"
#include "Material/Texture2D.h"
#include "Utilities/Singleton.inl"
#include "Material/Material.h"


#ifndef CXC_TEXTURE_MANAGER_H
#define CXC_TEXTURE_MANAGER_H

namespace cxc {

	enum class TextureUnit : uint16_t {
		UserTextureUnit,
		ShadowTextureUnit
	};

	class CXC_ENGINECORE_API MaterialManager final : public Singleton<MaterialManager>
	{

	public:

		friend Singleton<MaterialManager>;
		friend class Mesh ;

		explicit MaterialManager();
		~MaterialManager();

		MaterialManager(const MaterialManager &) = delete;
		MaterialManager(const MaterialManager &&) = delete;
		MaterialManager& operator=(const MaterialManager &) = delete;
		MaterialManager& operator=(const MaterialManager &&) = delete;

	// Texture loading and unloading
	public:

		void addMaterial(std::shared_ptr<Material> pMaterial);
		std::shared_ptr<Material> GetMaterial(const std::string& MaterialName);
		std::shared_ptr<Material> GetMaterial(std::shared_ptr<Material> pMaterial);

	private:

		// <Material Name , Pointer to the texture>
		std::unordered_map<std::string , std::shared_ptr<Material>> CachedMaterials;
	};

}

#endif // CXC_TEXTURE_MANAGER_H
