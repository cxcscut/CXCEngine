#include "..\General\EngineCompoents.h"
#include "..\General\DefineTypes.h"

#define _CRT_SECURE_NO_WARINGS

#ifndef CXC_ENGINEFACADE_H
#define CXC_ENGINEFACADE_H

#define MAX_DISTANCE 5000.0f
#define MIN_DISTANCE 500.0f
#define ZOOMING_SPEED 100.0f
#define MOUSE_ROTATE_ANGLE_SPEED 1.0f

#include "..\inl\Singleton.inl"

namespace cxc {

	// Input handler
	void KeyBoradCallBack(GLFWwindow *window, int key, int scancode, int action, int mods);
	void MouseCallBack(GLFWwindow *window, int button, int action, int mods);
	void CursorPosCallBack(GLFWwindow *window,double x,double y);
	void ScrollBarCallBack(GLFWwindow *window,double x,double y);

	// Window description
	typedef struct WindowDescriptor {
		GLint SamplingLevel;
		GLboolean isEnableDepth;
		GLboolean isForwardComptible;
		Color BackGroundColor;

		bool operator ==(const WindowDescriptor &windes) {
			return SamplingLevel == windes.SamplingLevel
				&& isEnableDepth == windes.isEnableDepth
				&& isForwardComptible == windes.isForwardComptible
				&& BackGroundColor == windes.BackGroundColor;
		}
	} WindowDescriptor;

	class EngineFacade final : public Singleton<EngineFacade>
	{
	public:

		friend Singleton<EngineFacade>;

		explicit EngineFacade();
		~EngineFacade();

		EngineFacade(const EngineFacade &) = delete;
		EngineFacade(const EngineFacade &&) = delete;

		EngineFacade& operator=(const EngineFacade &) = delete;
		EngineFacade& operator=(const EngineFacade &&) = delete;

	public:

		GLboolean CreateAndDisplayWindow(GLint Height,GLint Width,const std::string Title);

		GLboolean LoadShader(ShaderType Type,const std::string &vertex_shader_path,const std::string &fragment_shader_path);

	// Configuration
	public: 

		void SetBackGroundColor(float red, float green, float blue, float alpha) noexcept;
		void SetWindowParams(const WindowDescriptor &windes) noexcept;
		void SetGraphicsLibVersion(GLint HighByte,GLint LowByte) noexcept;
		void SetLightPos(const glm::vec3 &light_pos) noexcept;
		void SetCameraParams(const glm::vec3 &eye,const glm::vec3 &origin,const glm::vec3 &up,
							const glm::mat4 &ProjectionMatrix) noexcept;
		void SetCameraMode(CameraModeType mode) noexcept;

	public:

		void InitCameraStatus() noexcept;
		void InitEngine() noexcept;
		void CleanFrameBuffer() const noexcept;
		void ActivateRenderer(ShaderType Type) const noexcept;
		void CleanGL() noexcept {glfwTerminate();};
		void StoreAndSetMousePos() noexcept;
		void UpdateCameraPos() noexcept;

		void BindCameraUniforms() const noexcept;

		void RenderingScenes() const noexcept;
		void GameLooping() noexcept;

		void run() noexcept;
		void waitForStop() noexcept;

	// Data access interface
	public:

		std::shared_ptr<InputManager> GetInputManagerPtr() const noexcept { return m_pInputMgr;}
		std::shared_ptr<RendererManager> GetRendermanagerPtr() const noexcept {return m_pRendererMgr;}
		std::shared_ptr<Camera> GetCameraPtr() const noexcept {return m_pCamera;}
		std::shared_ptr<WindowManager> GetWindowMangaerPtr() const noexcept { return m_pWindowMgr; }
		std::shared_ptr<SceneManager> GetSceneManagerPtr() const noexcept { return m_pSceneMgr; }

		std::string GetVertexShaderPath() const noexcept { return VertexShaderPath; }
		std::string GetFragmentShaderPath() const noexcept { return FragmentShaderPath; }

	public:

		// flag representing the status
		GLboolean GameOver;

	private:

		// Pointer to the manager
		std::shared_ptr<InputManager> m_pInputMgr;
		std::shared_ptr<RendererManager> m_pRendererMgr;
		std::shared_ptr<Camera> m_pCamera;
		std::shared_ptr<WindowManager> m_pWindowMgr;
		std::shared_ptr<SceneManager> m_pSceneMgr;

		// Shader path
		std::string VertexShaderPath,FragmentShaderPath;

		// Rendering thread
		std::unique_ptr<std::thread> m_RenderingThread;
	};
}

#endif // CXC_ENGINEFACADE_H
