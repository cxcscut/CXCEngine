#include "TextureManager.h"

namespace cxc {

	TextureManager::TextureManager()
	{

	}

	TextureManager::~TextureManager()
	{

	}

	bool TextureManager::IsTextureExist(const std::string& TexName)
	{
		auto iter = CachedTextures.find(TexName);
		if (iter != CachedTextures.end())
			return true;
		else
			return false;
	}

	std::shared_ptr<Texture2D> TextureManager::LoadTexture(const std::string& TexName, const std::string& TexFileName)
	{
		// Create texture instance
		auto Tex2DInstance = std::make_shared<Texture2D>(TexFileName);
		if (Tex2DInstance)
		{
			CachedTextures.insert(std::make_pair(TexName, Tex2DInstance));
			return Tex2DInstance;
		}
		else
			return std::shared_ptr<Texture2D>(nullptr);
	}
}