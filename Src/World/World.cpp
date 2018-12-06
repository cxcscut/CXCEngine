#include "World/World.h"
#include "Scene/SceneManager.h"

namespace cxc {

	World::World()
		:
		GameOver(GL_FALSE),
		RenderingFrameRates(60)
	{
		WorldStartSeconds = std::chrono::system_clock::now();
		m_LogicFramework = LogicFramework::GetInstance();
		m_PhysicalWorld = PhysicalWorld::GetInstance();

		pInputMgr = InputManager::GetInstance();
		pWindowMgr = WindowManager::GetInstance();
		pSceneMgr = SceneManager::GetInstance();
	}

	World::~World()
	{

	}

	void World::AddObject(const std::shared_ptr<Object3D > &pObject, bool isKinematics) noexcept
	{
		if (pObject)
		{
			pSceneMgr->AddObjectInternal(pObject->GetObjectName(), pObject, isKinematics);

			if(!pObject->IsInitialize())
				pObject->InitializeRigidBody(m_PhysicalWorld->GetWorldID(), m_PhysicalWorld->GetTopSpaceID());
		}
	}

	GLboolean World::CreateAndDisplayWindow(GLint Width, GLint Height, const std::string Title)
	{
		pWindowMgr->SetWindowHeight(Height);
		pWindowMgr->SetWindowWidth(Width);
		pWindowMgr->SetWindowTitle(Title);

		if (!pWindowMgr->PrepareResourcesForCreating())
			return GL_FALSE;

		if (!pWindowMgr->_CreateWindow()){
			glfwTerminate();
			return GL_FALSE;
		}

		pWindowMgr->InitContext();

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			std::cerr << "GLEW initialization failed" << std::endl;
			return GL_FALSE;
		}

		return GL_TRUE;
	}

	void World::SetGravity(GLfloat x, GLfloat y, GLfloat z) noexcept
	{
		auto InGravity = glm::vec3({ x,y,z });

		assert(m_PhysicalWorld != nullptr);
		m_PhysicalWorld->SetGravity(InGravity);
	}

	void World::SetWindowParams(const WindowDescriptor &windes) noexcept
	{
		pWindowMgr->SetEnableDepthFlag(windes.isEnableDepth);
		pWindowMgr->SetForwardCompatibleFlag(windes.isForwardComptible);
		pWindowMgr->SetSamplingLevel(windes.SamplingLevel);
		pWindowMgr->SetBackGroundColor(windes.BackGroundColor.Red,
											windes.BackGroundColor.Green,
											windes.BackGroundColor.Blue,
											windes.BackGroundColor.Alpha);
	}

	void World::Initialize() noexcept
	{
		pWindowMgr->InitGL();

		// Create window
		if (!CreateAndDisplayWindow(pWindowMgr->GetWindowWidth(), pWindowMgr->GetWindowHeight(), pWindowMgr->GetWindowTitle()))
		{
			std::cerr << "Failed to create display window" << std::endl;

			// shutdown and clean
			glfwTerminate();
			return;
		}

		// Init input mode
		InitInputMode();

		// Turn on the vsync
		glfwSwapInterval(1);

		m_PhysicalWorld->InitializePhysicalWorld();
	}
 
	float World::GetWorldSeconds() const
	{
		auto CurrentSeconds = std::chrono::system_clock::now();
		auto Duration = std::chrono::duration_cast<std::chrono::microseconds>(CurrentSeconds - WorldStartSeconds);
		return  float(Duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
	}

	void World::run() noexcept
	{

		// Contruct octree
		// pSceneMgr->BuildOctree();

		// Initialize the start time of the world
		WorldStartSeconds = std::chrono::system_clock::now();
		auto CurrentWorldSeconds = GetWorldSeconds();
		LastLogicWorldTickSeconds = CurrentWorldSeconds;
		LastPhysicalWorldTickSeconds = CurrentWorldSeconds;

		// Begin event looping
		WorldLooping();

		// Shutdown GL context
		CleanGL();
	}

	void World::SetGraphicsLibVersion(GLint HighByte, GLint LowByte) noexcept
	{
		pWindowMgr->SetDriverVersion(HighByte,LowByte);
	}

	void World::InitInputMode() noexcept
	{
		glEnable(GL_CULL_FACE);
		pInputMgr->InitializeInput(pWindowMgr->GetWindowHandle());
	}

	void World::CleanFrameBuffer() const noexcept
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		if (pWindowMgr->isEnableDepthTest())
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else
			glClear(GL_COLOR_BUFFER_BIT);
	}

	void World::SetBackGroundColor(float red, float green, float blue, float alpha) noexcept
	{
		pWindowMgr->SetBackGroundColor(red,green,blue,alpha);
	}

	void World::RenderingTick()
	{
		assert(pSceneMgr != nullptr);
		auto FixedRenderingDeltaSeconds = 1 / static_cast<float>(RenderingFrameRates);
		pSceneMgr->Tick(FixedRenderingDeltaSeconds);
	}

	void World::ProcessInput()
	{
		// Input ticking for every rendering frame
		assert(pSceneMgr != nullptr);
		auto FixedRenderingDeltaSeconds = 1 / static_cast<float>(RenderingFrameRates);

		// Tick the InputManager
		InputManager::GetInstance()->Tick(FixedRenderingDeltaSeconds);
	}

	void World::LogicFrameworkTick()
	{
		assert(m_LogicFramework != nullptr);

		auto CurrentWorldSeconds = GetWorldSeconds();
		auto SecondsBetweenFrames = CurrentWorldSeconds - LastLogicWorldTickSeconds;
		auto FixedLogicDeltaSeconds = 1 / static_cast<float>(m_LogicFramework->GetLogicFrameRates());
		if (SecondsBetweenFrames >= FixedLogicDeltaSeconds)
		{
			LastLogicWorldTickSeconds = CurrentWorldSeconds - SecondsBetweenFrames + FixedLogicDeltaSeconds;

			m_LogicFramework->LogicTick(FixedLogicDeltaSeconds);
		}
	}

	void World::PhysicsTick()
	{
		assert(m_PhysicalWorld != nullptr);

		// The frame rates of the physical world equals to logic world
		auto CurrentWorldSeconds = GetWorldSeconds();
		auto SecondsBetweenFrames = CurrentWorldSeconds - LastPhysicalWorldTickSeconds;
		auto FixedLogicDeltaSeconds = 1 / static_cast<float>(m_LogicFramework->GetLogicFrameRates());
		if (SecondsBetweenFrames < FixedLogicDeltaSeconds)
		{
			LastPhysicalWorldTickSeconds = CurrentWorldSeconds - SecondsBetweenFrames + FixedLogicDeltaSeconds;

			m_PhysicalWorld->PhysicsTick(FixedLogicDeltaSeconds);
		}
	}

	void World::Tick()
	{
		// Rendering tick
		RenderingTick();

		// Physics tick
		PhysicsTick();

		// Logic tick
		LogicFrameworkTick();

		// Processing the input
		ProcessInput();
	}

	void World::WorldLooping() noexcept
	{
		auto WindowHandler = pWindowMgr->GetWindowHandle();
		do {

			// clean frame buffer for rendering
			CleanFrameBuffer();

			// World tick
			Tick();

			// Swap front and back buffer
			glfwSwapBuffers(WindowHandler);

			glfwPollEvents();

		} while (!GameOver && glfwWindowShouldClose(WindowHandler) == 0);

		GameOver = true;

	}
}


