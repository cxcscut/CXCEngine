#include "..\General\DefineTypes.h"

#ifndef CXC_SceneManager_H
#define CXC_SceneManager_H

#define WOLRD_QUICK_STEPSIZE 0.05f

#include "..\inl\Singleton.inl"
#include "..\Graphics\Object3D.h"
#include "..\Graphics\Shape.h"
#include "..\Graphics\TextureManager.h"
#include "..\Controller\Camera.h"

namespace cxc {

	enum class Location : GLuint {
		VERTEX_LOCATION = 0,
		TEXTURE_LOCATION = 1,
		NORMAL_LOCATION = 2,
		COLOR_LOCATION = 3,
		NUM_OF_LOCATION = 4
	};

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

		// Delete object
		void DeleteObject(const std::string &sprite_name) noexcept;

		// Add object to object map
		void AddObject(const std::string &SpriteName,const std::shared_ptr<Object3D > &SpritePtr,bool isKinematics = false) noexcept;

	// Data access interface
	public:

		// Pointer to resource manager
		std::shared_ptr<TextureManager> m_pTextureMgr;
		std::shared_ptr<Camera> m_pCamera;
		std::shared_ptr<RendererManager> m_pRendererMgr;

		// Return the pointer to the specific sprite
		std::shared_ptr<Object3D > GetObject3D (const std::string &sprite_name) const noexcept;

		// Camera interface
		void InitCameraStatus(GLFWwindow * window) noexcept;
		void SetCameraParams(const glm::vec3 &eye, const glm::vec3 &origin, const glm::vec3 &up,
							const glm::mat4 &ProjectionMatrix) noexcept;
		void UpdateCameraPos(GLFWwindow *window, float x, float y, GLuint height, GLuint width) noexcept;
		void BindCameraUniforms() const noexcept;
		void SetCameraMode(CameraModeType mode) noexcept;

		// Return the SpriteMap
		const std::unordered_map<std::string, std::shared_ptr<Object3D >> &GetObjectMap() const noexcept;

		// Lighting interface
		const glm::vec3 &GetLightPos() const noexcept;
		void BindLightingUniforms(GLuint ProgramID) const;
		void SetLightPos(const glm::vec3 &pos) noexcept;

	// Draw call and resource management
	public:

		void DrawScene() noexcept;

		void initResources() noexcept;

		void releaseBuffers() noexcept;

	// Physics settings
	public:

		void CreatePhysicalWorld(const glm::vec3 & gravity) noexcept;

		static void nearCallback(void *data, dGeomID o1, dGeomID o2) noexcept;

		void UpdateMeshTransMatrix() noexcept;

		// physics world
		dWorldID m_WorldID;

		// Top level space
		dSpaceID m_TopLevelSpace;

		// Joint group 
		dJointGroupID m_ContactJoints;

	private:

		// <Object Name , Pointer to object>
		std::unordered_map<std::string, std::shared_ptr<Object3D>> m_ObjectMap;

		// Buffer object ID
		GLuint VAO, EBO, VBO_P, VBO_A;

		// Vertex indices number
		uint32_t TotalIndicesNum;

		// Light position
		glm::vec3 m_LightPos;

	};

}
#endif // CXC_SceneManager_H
