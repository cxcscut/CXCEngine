#include "Core/EngineTypes.h"
#include "Texture2D.h"
#include "Utilities/Singleton.inl"

#ifndef CXC_TEXTUREMANAGER_H
#define CXC_TEXTUREMANAGER_H

namespace cxc
{

	class CXC_ENGINECORE_API TextureManager final : public Singleton<TextureManager>
	{
		friend class Singleton<TextureManager>;

	public:

		explicit TextureManager();
		~TextureManager();

		TextureManager(const TextureManager &) = delete;
		TextureManager(const TextureManager &&) = delete;
		TextureManager& operator=(const TextureManager &) = delete;
		TextureManager& operator=(const TextureManager &&) = delete;

		bool IsTextureExist(const std::string& TexName);
		std::shared_ptr<Texture2D> LoadTexture(const std::string& TexName, const std::string& TexFileName);

	public:

		/* Mapping from texture name to texture instance */
		std::unordered_map<std::string, std::shared_ptr<Texture2D>> CachedTextures;

	};

}

#endif // CXC_TEXTUREMANAGER_H
