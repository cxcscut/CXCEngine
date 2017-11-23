#include "..\General\DefineTypes.h"

#ifndef CXC_SceneManager_H
#define CXC_SceneManager_H

#include "..\inl\Singleton.inl"
#include "..\Graphics\Object3D.h"
#include "..\Graphics\Shape.h"
#include "..\Graphics\TextureManager.h"
#include "..\Controller\Camera.h"

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

		std::shared_ptr<TextureManager> m_pTextureMgr;
		std::shared_ptr<Camera> m_pCamera;
		std::shared_ptr<RendererManager> m_pRendererMgr;

		// Return the pointer to the specific sprite
		std::shared_ptr<Object3D > GetObject3D (const std::string &sprite_name) const noexcept;

		void InitCameraStatus(GLFWwindow * window) noexcept;

		void SetCameraParams(const glm::vec3 &eye, const glm::vec3 &origin, const glm::vec3 &up,
			const glm::mat4 &ProjectionMatrix) noexcept;

		void UpdateCameraPos(GLFWwindow *window, float x, float y, GLuint height, GLuint width) noexcept;

		void BindCameraUniforms() const noexcept;

		void SetCameraMode(CameraModeType mode) noexcept;

		// Return the SpriteMap
		const std::unordered_map<std::string, std::shared_ptr<Object3D >> &GetObjectMap() const noexcept;

		const glm::vec3 &GetLightPos() const noexcept;
		void BindLightingUniforms(GLuint ProgramID) const;
		void SetLightPos(const glm::vec3 &pos) noexcept;

	// Draw call
	public:

		void DrawScene() noexcept;

		void initResources() noexcept;

		void releaseBuffers() noexcept;

	private:

		

		// <Object Name , Pointer to object>
		std::unordered_map<std::string, std::shared_ptr<Object3D>> m_ObjectMap;

		GLuint VAO, EBO, VBO_P, VBO_A;

		uint32_t TotalIndicesNum;

		glm::vec3 m_LightPos;
	};
}
#endif // CXC_SceneManager_H
