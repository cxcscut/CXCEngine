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

	public:

		void InitEngine() noexcept;
		void CleanFrameBuffer() const noexcept;
		void ActivateRenderer(ShaderType Type) const noexcept;
		void CleanGL() noexcept {glfwTerminate();};
		void StoreAndSetMousePos() noexcept;

		void RenderingScenes() const noexcept;
		void GameLooping() noexcept;

		void run(GLboolean Multithreading = GL_TRUE) noexcept;
		void waitForStop() noexcept;

	// Data access interface
	public:

		// Pointer to the manager
		std::shared_ptr<InputManager> m_pInputMgr;
		std::shared_ptr<WindowManager> m_pWindowMgr;
		std::shared_ptr<SceneManager> m_pSceneMgr;

		// Shader path
		std::string GetVertexShaderPath() const noexcept { return VertexShaderPath; }
		std::string GetFragmentShaderPath() const noexcept { return FragmentShaderPath; }
		void SetVertexShaderPath(const std::string &_VertexShaderPath) noexcept { VertexShaderPath = _VertexShaderPath; };
		void SetFragmentShaderPath(const std::string &_FragmentShaderPath) noexcept { FragmentShaderPath = _FragmentShaderPath; };

	public:

		// flag representing the status
		GLboolean GameOver;

	private:

		// Shader path
		std::string VertexShaderPath,FragmentShaderPath;

		// Rendering thread
		std::unique_ptr<std::thread> m_RenderingThread;
	};
}

#endif // CXC_ENGINEFACADE_H
