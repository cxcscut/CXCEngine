#ifdef WIN32

#include "..\General\DefineTypes.h"

#include "..\Graphics\Texture2D.h"
#include "..\inl\Singleton.inl"

#else

#include "../General/DefineTypes.h"

#include "../Graphics/Texture2D.h"
#include "../inl/Singleton.inl"
#endif // WIN32

#ifndef CXC_TEXTURE_MANAGER_H
#define CXC_TEXTURE_MANAGER_H

namespace cxc {

	class TextureManager final : public Singleton<TextureManager>
	{

		enum class TextureUnit : uint16_t {
			UserTextureUnit,
			ShadowTextureUnit
		};

	public:

		friend Singleton<TextureManager>;
		friend class Object3D ;

		explicit TextureManager();
		~TextureManager();

		TextureManager(const TextureManager &) = delete;
		TextureManager(const TextureManager &&) = delete;
		TextureManager& operator=(const TextureManager &) = delete;
		TextureManager& operator=(const TextureManager &&) = delete;

	// Texture loading and unloading
	public:

		GLboolean LoadTexture(const std::string &tex_name, const std::string &tex_path);
		void addTexture(const std::string &tex_name,const std::string &tex_path);
		GLboolean DeleteTexture(const std::string &tex_name) noexcept;

		void LoadAllTexture() noexcept;

	// Data access interface
	public:

		GLuint GetTextureID(const std::string &tex_name) const noexcept;

		std::shared_ptr<Texture2D> GetTexPtr(const std::string &tex_name) const noexcept;
	// Resource acquisition and release
	public:

		void RemoveAllTexture() noexcept;
		void ReloadAllTexture() noexcept;

	private:

		// <Texture name , Pointer to the texture>
		std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_TextureMap;

		//  <Texture name, Texture file path>
		std::unordered_map<std::string, std::string> m_TextPathList;

		// Textures queue
		std::deque<std::pair<std::string, std::string>> m_LoadingQueue;
	};

}
#endif // CXC_TEXTURE_MANAGER_H
