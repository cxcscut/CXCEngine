#include "..\General\DefineTypes.h"

#ifndef CXC_SceneManager_H
#define CXC_SceneManager_H

#include "..\inl\Singleton.inl"
#include "..\Graphics\Object3D.h"
#include "..\Graphics\Shape.h"
#include "..\Graphics\TextureManager.h"

namespace cxc {

	class SceneManager final : public Singleton<SceneManager>
	{

	public:

		friend Singleton<SceneManager>;

		explicit SceneManager();
		~SceneManager();
		
		SceneManager(const SceneManager&) = delete;
		SceneManager(const SceneManager&&) = delete;
		SceneManager& operator=(const SceneManager&) = delete;
		SceneManager& operator=(const SceneManager&&) = delete;

	// Sprite creation
	public:

		// Create sprite from file
		GLboolean CreateObject(const std::string &sprite_name,const std::string &sprite_file) noexcept;

		void DeleteObject(const std::string &sprite_name) noexcept;

		void AddObject(const std::string &SpriteName,const std::shared_ptr<Object3D > &SpritePtr) noexcept;

	// Data access interface
	public:

		// Return the pointer to the specific sprite
		std::shared_ptr<Object3D > GetObject3D (const std::string &sprite_name) const noexcept;

		// Return the SpriteMap
		const std::unordered_map<std::string, std::shared_ptr<Object3D >> &GetObjectMap() const noexcept;

		std::shared_ptr<TextureManager> GetTextureManagerPtr() const noexcept { return m_pTextureMgr; }

	// Draw call
	public:

		void DrawScene() noexcept;

		void initResources() noexcept;

		void releaseBuffers() noexcept;

	private:

		std::shared_ptr<TextureManager> m_pTextureMgr;

		// <Object Name , Pointer to object>
		std::unordered_map<std::string, std::shared_ptr<Object3D>> m_ObjectMap;

		GLuint VAO, EBO, VBO_P, VBO_A;

		uint32_t TotalIndicesNum;

	};
}
#endif // CXC_SceneManager_H
