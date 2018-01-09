#include "TextureManager.h"

#ifdef WIN32

#include "..\Controller\RendererManager.h"

#else

#include "../Controller/RendererManager.h"

#endif // WIN32

namespace cxc {

	TextureManager::TextureManager()
		:m_TextureMap(),
		m_TextPathList()
	{

	}

	TextureManager::~TextureManager()
	{
		m_TextPathList.clear();
		m_TextureMap.clear();
	}

	void TextureManager::addTexture(const std::string &tex_name, const std::string &tex_path)
	{
		m_LoadingQueue.emplace_back(std::make_pair(tex_name,tex_path));
	}

	std::shared_ptr<Texture2D> TextureManager::GetTexPtr(const std::string &tex_name) const noexcept
	{
		auto it = m_TextureMap.find(tex_name);
		if (it != m_TextureMap.end())
			return it->second;
		else
			return nullptr;
	}

	void TextureManager::LoadAllTexture() noexcept
	{
		while (!m_LoadingQueue.empty())
		{
			auto texture = m_LoadingQueue.front();
			LoadTexture(texture.first,texture.second);
			m_LoadingQueue.pop_front();
		}
	}

	GLboolean TextureManager::LoadTexture(const std::string &tex_name, const std::string &tex_path)
	{
		if (m_TextureMap.find(tex_name) != m_TextureMap.end())
			return GL_FALSE;

		auto nameit = m_TextPathList.find(tex_name);
		if (nameit != m_TextPathList.end())
			nameit->second = tex_path;
		else
			m_TextPathList[tex_name] = tex_path;

		auto tex_tmp = std::make_shared<Texture2D>(tex_path);
		if (!tex_tmp->GetTextureID())
			return GL_FALSE;

		m_TextureMap[tex_name] = std::move(tex_tmp);
		return GL_TRUE;
	}

	GLboolean TextureManager::DeleteTexture(const std::string &tex_name) noexcept
	{
		auto it = m_TextureMap.find(tex_name);
		if (it != m_TextureMap.end())
		{
			it->second->releaseTexture();
			m_TextureMap.erase(it);
			return GL_TRUE;
		}
		return GL_FALSE;
	}

	void TextureManager::RemoveAllTexture() noexcept
	{
		for (auto it : m_TextureMap)
			it.second->releaseTexture();

		m_TextureMap.clear();
	}

	void TextureManager::ReloadAllTexture() noexcept
	{
		m_TextureMap.clear();
		for (const auto& it : m_TextPathList)
			m_TextureMap[it.first] = std::make_shared<Texture2D>(it.second);

	}

	GLuint TextureManager::GetTextureID(const std::string &tex_name) const noexcept
	{
		auto tex = m_TextureMap.find(tex_name);
		if (tex != m_TextureMap.end())
			return tex->second->GetTextureID();
		else
			return 0;
	}

}
