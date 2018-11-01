#ifdef WIN32

#include "..\General\DefineTypes.h"

#include "..\Graphics\Texture2D.h"
#include "..\inl\Singleton.inl"
#include "..\Graphics\Material.h"

#else

#include "../General/DefineTypes.h"

#include "../Graphics/Texture2D.h"
#include "../inl/Singleton.inl"
#include "../Graphics/Material.h"

#endif // WIN32

#ifndef CXC_TEXTURE_MANAGER_H
#define CXC_TEXTURE_MANAGER_H

namespace cxc {

	enum class TextureUnit : uint16_t {
		UserTextureUnit,
		ShadowTextureUnit
	};

	class MaterialManager final : public Singleton<MaterialManager>
	{

	public:

		friend Singleton<MaterialManager>;
		friend class Object3D ;

		explicit MaterialManager();
		~MaterialManager();

		MaterialManager(const MaterialManager &) = delete;
		MaterialManager(const MaterialManager &&) = delete;
		MaterialManager& operator=(const MaterialManager &) = delete;
		MaterialManager& operator=(const MaterialManager &&) = delete;

	// Texture loading and unloading
	public:

		void addMaterial(std::shared_ptr<Material> pMaterial);

	private:

		// <Material Name , Pointer to the texture>
		std::unordered_map<std::string , std::shared_ptr<Material>> CachedMaterials;
	};

}

#endif // CXC_TEXTURE_MANAGER_H
