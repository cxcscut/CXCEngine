#include "Core/EngineCore.h"
#include "Actor/CCameraActor.h"
#include "Actor/CLightActor.h"
#include "Rendering/RendererManager.h"
#include "Controller/InputManager.h"
#include "Material/MaterialManager.h"
#include "Scene/SceneManager.h"
#include "Window/Window.h"
#include "Script/ScriptParser.h"
#include "Core/EngineTypes.h"
#include "Utilities/Singleton.inl"
#include "World/LogicFramework.h"
#include "Physics/PhysicalWorld.h"

#define _CRT_SECURE_NO_WARINGS

#ifndef CXC_WORLD_H
#define CXC_WORLD_H

#define MAX_DISTANCE 5000.0f
#define MIN_DISTANCE 50.0f
#define ZOOMING_SPEED 100.0f
#define MOUSE_ROTATE_ANGLE_SPEED 1.0f

namespace cxc {

	class SceneContext;

	const float FixedDeltaSeconds_30FPS = 0.3333f;
	const float FixedDeltaSeconds_60FPS = 0.1667f;

	using SystemClock = std::chrono::time_point<std::chrono::system_clock>;

	// Input handler
	void KeyBoradCallBack(GLFWwindow *window, int key, int scancode, int action, int mods);
	void MouseCallBack(GLFWwindow *window, int button, int action, int mods);
	void CursorPosCallBack(GLFWwindow *window,double x,double y);

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

	class CXC_ENGINECORE_API World final : public Singleton<World>, public std::enable_shared_from_this<World>
	{
	public:

		friend Singleton<World>;

		explicit World();
		~World();

		World(const World &) = delete;
		World(const World &&) = delete;

		World& operator=(const World &) = delete;
		World& operator=(const World &&) = delete;

	public:

		GLboolean CreateAndDisplayWindow(GLint Height,GLint Width,const std::string Title);

	public:

		void SetRenderingFrameRates(uint32_t NewFrameRates) { RenderingFrameRates = NewFrameRates; }
		uint32_t GetRenderingFrameRates() const { return RenderingFrameRates; }
		void SetBackGroundColor(float red, float green, float blue, float alpha) noexcept;
		void SetWindowParams(const WindowDescriptor &windes) noexcept;
		void SetGraphicsLibVersion(GLint HighByte,GLint LowByte) noexcept;
		void SetWindowPosition(GLint x, GLint y) noexcept { pWindowMgr->x_pos = x; pWindowMgr->y_pos = y; };

	public:

		void Initialize() noexcept;
		void InitInputMode() noexcept;

	public:

		void CleanFrameBuffer() const noexcept;
		void CleanGL() noexcept {glfwTerminate();};

		void WorldLooping() noexcept;

		void StartToRun() noexcept;

	public:

		void AddActor(std::shared_ptr<CActor> Actor);
		void RemoveActor(std::shared_ptr<CActor> Actor);
		void RemoveActor(uint32_t GUID);
		void RemoveActor(const std::string& ActorName);

	public:

		bool LoadSceneFromFBX(const std::string& filepath, std::shared_ptr<SceneContext> OutSceneContext) noexcept;
		void ProcessSceneNode(FbxNode* pRootNode, std::shared_ptr<SceneContext> OutSceneContext)  noexcept;

	public:

		void Tick();
		void RenderingTick(float DeltaSeconds);

		void LogicTick();
		void PhysicsTick(float DeltaSeconds);
		void ProcessInput();

		// Time
	public:

		float GetWorldSeconds() const;
		std::shared_ptr<LogicFramework> GetLogicWorld() { return m_LogicFramework; }
		std::shared_ptr<PhysicalWorld> GetPhysicalWorld() { return m_PhysicalWorld; }

	public:

		// Pointer to the manager
		std::shared_ptr<InputManager> pInputMgr;
		std::shared_ptr<WindowManager> pWindowMgr;
		std::shared_ptr<SceneManager> pSceneMgr;

	// Physics
	public:

		void SetGravity(GLfloat x,GLfloat y,GLfloat z) noexcept;

	public:

		// flag representing the status
		GLboolean GameOver;

	private:

		// Rendering frame rates
		uint32_t RenderingFrameRates;

		// Start time of the engine
		SystemClock WorldStartSeconds;

		float LastLogicWorldTickSeconds;
			
		float LastRenderingTickSeconds;

		// Logic framework
		std::shared_ptr<LogicFramework> m_LogicFramework;

		// Physical world
		std::shared_ptr<PhysicalWorld> m_PhysicalWorld;
	};
}

#endif // CXC_WORLD_H
