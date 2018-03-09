#ifdef WIN32

#include "..\General\EngineCompoents.h"
#include "..\General\DefineTypes.h"
#include "..\inl\Singleton.inl"

#else

#include "../General/EngineCompoents.h"
#include "../General/DefineTypes.h"
#include "../inl/Singleton.inl"

#endif // WIN32

#include <condition_variable>

#define _CRT_SECURE_NO_WARINGS

#ifndef CXC_ENGINEFACADE_H
#define CXC_ENGINEFACADE_H

#define MAX_DISTANCE 5000.0f
#define MIN_DISTANCE 50.0f
#define ZOOMING_SPEED 100.0f
#define MOUSE_ROTATE_ANGLE_SPEED 1.0f

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

		void addObject(const std::shared_ptr<Object3D > &pObject,bool isKinematics = false) noexcept;

		void MultiThreadingEnable() noexcept { MultiThreading = true; };

	public:

		GLboolean CreateAndDisplayWindow(GLint Height,GLint Width,const std::string Title);

		void addShader(const std::string &name,BaseRender *Render);

	// Configuration
	public:

		void SetBackGroundColor(float red, float green, float blue, float alpha) noexcept;
		void SetWindowParams(const WindowDescriptor &windes) noexcept;
		void SetGraphicsLibVersion(GLint HighByte,GLint LowByte) noexcept;
		void InitWindowPosition(GLint x, GLint y) noexcept { m_pWindowMgr->x_pos = x; m_pWindowMgr->y_pos = y; };

	public:

		void Init() noexcept;
		void InitEngine() noexcept;
		void CleanFrameBuffer() const noexcept;
		void CleanGL() noexcept {glfwTerminate();};
		void StoreAndSetMousePos() noexcept;

		void RenderingScenes() noexcept;
		void GameLooping() noexcept;

		void run() noexcept;
		void waitForStop() noexcept;

	// Data access interface
	public:

		// Pointer to the manager
		std::shared_ptr<InputManager> m_pInputMgr;
		std::shared_ptr<WindowManager> m_pWindowMgr;
		std::shared_ptr<SceneManager> m_pSceneMgr;

		// Shader path
		void SetSceneSize(const glm::vec3 &center, float size) noexcept { m_pSceneMgr->SetCenter(center); m_pSceneMgr->SetSize(size); };
	// Physics
	public:

		void SetGravity(GLfloat x,GLfloat y,GLfloat z) noexcept;

		void ProcessingPhysics() noexcept;

		void SuspendPhysics() noexcept { pause = true; };

		void ResumePhysics() noexcept { pause = false; };

	public:

		// flag representing the status
		GLboolean GameOver;

		std::vector<std::pair<std::string, BaseRender*>> Renders;

	public:

		bool EngineInitilized;

		std::condition_variable cv;

		std::mutex m_InitLock,m_RunLock;

		glm::vec3 m_Gravity;

		bool ODE_initialized;

		bool MultiThreading;

		static std::function<void(int key, int scancode, int action, int mods)> KeyInputCallBack;

	private:

		// Rendering thread
		std::unique_ptr<std::thread> m_RenderingThread;

		bool pause;
	};
}

#endif // CXC_ENGINEFACADE_H
