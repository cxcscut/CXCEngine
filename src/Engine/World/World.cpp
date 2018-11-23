#include "World.h"

#ifdef WIN32

#include "..\Graphics\SceneManager.h"

#else

#include "../Graphics/SceneManager.h"

#endif // WIN32

namespace cxc {

	static double _x=0.0f, _y=0.0f;

	std::function<void(int key, int scancode, int action, int mods)>
		World::KeyInputCallBack = [=](int,int,int,int) {};

	void KeyBoradCallBack(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		World::KeyInputCallBack(key,scancode,action,mods);
	}

	void CursorPosCallBack(GLFWwindow *window, double x, double y)
	{
		auto pEngine = World::GetInstance();
		auto pRender = pEngine->pSceneMgr->pRenderMgr->GetCurrentUsedRender();
		auto CurrentUsedPipeline = pRender->GetCurrentUsedPipeline();
		if (!pRender || !CurrentUsedPipeline) return;

		auto ProgramID = CurrentUsedPipeline->GetPipelineProgramID();
		auto wHandle = pEngine->pWindowMgr->GetWindowHandle();

		auto camera = pEngine->pSceneMgr->pCamera;

		double dx = x-_x, dy = y-_y;
		_x = x; _y = y;

		double DeltaThetaXOY = -PI * (dx / pEngine->pWindowMgr->GetWindowWidth() / 2);
		double DeltaThetaToXOY = PI * (dy / pEngine->pWindowMgr->GetWindowHeight() / 2);

		// Update angles
		camera->ComputeAngles();

		// Rotate Camera
		camera->ThetaToXOY += DeltaThetaToXOY;
		camera->ThetaXOY += DeltaThetaXOY;

		camera->ComputePosition();
		camera->ComputeViewMatrix();
		camera->BindViewMatrix(ProgramID);

		//std::cout << "Camera pos : " << camera->EyePosition.x << "," << camera->EyePosition.y << "," << camera->EyePosition.z << std::endl;
		//std::cout << "Looking at : " << camera->CameraOrigin.x << "," << camera->CameraOrigin.y << "," << camera->CameraOrigin.z << std::endl;
	}

	void MouseCallBack(GLFWwindow *window, int button, int action, int mods)
	{
		auto pEngine = World::GetInstance();
		auto wHandle = pEngine->pWindowMgr->GetWindowHandle();
		if (!wHandle)
			return;

		if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
			glfwGetCursorPos(wHandle,&_x,&_y);

			// Activate the mouse callback
			glfwSetCursorPosCallback(wHandle,CursorPosCallBack);
		}
		else if(action == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT)
		{
			// Remove the mouse callback
			glfwSetCursorPosCallback(wHandle, nullptr);
		}
	}

	void ScrollBarCallBack(GLFWwindow *window, double x, double y)
	{
		auto pEngine = World::GetInstance();
		auto wHandle = pEngine->pWindowMgr->GetWindowHandle();
		auto pRender = pEngine->pSceneMgr->pRenderMgr->GetCurrentUsedRender();
		auto CurrentUsedPipeline = pRender->GetCurrentUsedPipeline();
		if (!pRender || !CurrentUsedPipeline) return;

		auto CurrentProgramID = CurrentUsedPipeline->GetPipelineProgramID();
		auto camera = pEngine->pSceneMgr->pCamera;
		if (!wHandle) return;

		glm::vec3 eye_direction = glm::normalize(camera->CameraOrigin - camera->EyePosition);
		float distance = glm::length(camera->EyePosition - camera->CameraOrigin);

		// y > 0 when srcoll up , y < 0 when scroll down
		if (y>0)
		{
			// srcoll up
			if (distance > MIN_DISTANCE) {
				camera->EyePosition += ZOOMING_SPEED * eye_direction;
			}
		}
		else
		{

			// scroll down
			if (distance <= MAX_DISTANCE) {
				camera->EyePosition -= ZOOMING_SPEED  * eye_direction;
			}

		}

		camera->ComputeViewMatrix();
		camera->BindViewMatrix(CurrentProgramID);

		//std::cout << "Camera pos : " << camera->EyePosition.x << "," << camera->EyePosition.y << "," << camera->EyePosition.z << std::endl;
		//std::cout << "Looking at : " << camera->origin.x << "," << camera->origin.y << "," << camera->origin.z << std::endl;
	}

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

			pObject->InitializeRigidBody(m_PhysicalWorld->GetWorldID(), m_PhysicalWorld->GetTopSpaceID());
		}
	}

	GLboolean World::CreateAndDisplayWindow(GLint Width,
												GLint Height,
												const std::string Title)
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
			return GL_FALSE;
		}

		return GL_TRUE;
	}

	void World::AddRender(const std::string &name, std::shared_ptr<MeshRender> MeshRender)
	{
		RendersTobeLoaded.emplace_back(std::make_pair(name,MeshRender));
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

	void World::Init() noexcept
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

		// Load and compile shaders
		LoadRender();

		// Init input mode
		InitInputMode();

		// Turn on the vsync
		glfwSwapInterval(1);

		// Init camera params and set input callback func
		pSceneMgr->InitCameraStatus(pWindowMgr->GetWindowHandle());

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
		LastRenderingTickSeconds = CurrentWorldSeconds;
		LastLogicWorldTickSeconds = CurrentWorldSeconds;
		LastPhysicalWorldTickSeconds = CurrentWorldSeconds;

		// Begin event looping
		WorldLooping();

		// Shutdown GL context
		CleanGL();
	}

	void World::LoadRender()
	{
		if (RendersTobeLoaded.empty())
			return;
		else
		{
			while (!RendersTobeLoaded.empty())
			{
				auto pair = RendersTobeLoaded.back();
				RendersTobeLoaded.pop_back();

				if (pair.second->InitializeRender())
				{
					pSceneMgr->pRenderMgr->addRender(pair.first, pair.second);
				}
				else
				{
					std::cerr << "Failed to create the render : " << pair.first << std::endl;

					glfwTerminate();
					return;
				}
			}
		}
	}

	void World::SetGraphicsLibVersion(GLint HighByte, GLint LowByte) noexcept
	{
		pWindowMgr->SetDriverVersion(HighByte,LowByte);
	}

	void World::InitInputMode() noexcept
	{
		glEnable(GL_CULL_FACE);
		pInputMgr->SetInputModel(pWindowMgr->GetWindowHandle());
	}

	void World::CleanFrameBuffer() const noexcept
	{
		if (pWindowMgr->isEnableDepthTest())
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else
			glClear(GL_COLOR_BUFFER_BIT);
	}

	void World::StoreAndSetMousePos() noexcept
	{
		pInputMgr->UpdateMousePos(pWindowMgr->GetWindowHandle());
		pInputMgr->SetMouseScreenPos(pWindowMgr->GetWindowHandle(), pWindowMgr->GetWindowWidth() / 2, pWindowMgr->GetWindowHeight() / 2);
	}

	void World::SetBackGroundColor(float red, float green, float blue, float alpha) noexcept
	{
		pWindowMgr->SetBackGroundColor(red,green,blue,alpha);
	}

	void World::RenderingTick()
	{
		assert(pSceneMgr != nullptr);

		auto CurrentWorldSeconds = GetWorldSeconds();
		auto SecondsBetweenFrames = CurrentWorldSeconds - LastRenderingTickSeconds;
		auto FixedRenderingDeltaSeconds = 1 / static_cast<float>(RenderingFrameRates);
		if (SecondsBetweenFrames >= FixedRenderingDeltaSeconds)
		{
			//std::cout << "Rendering tick difference : " << SecondsBetweenFrames << std::endl;

			LastRenderingTickSeconds = CurrentWorldSeconds - SecondsBetweenFrames + FixedRenderingDeltaSeconds;
			
			pSceneMgr->Tick(SecondsBetweenFrames);
		}
	}

	void World::LogicFrameworkTick()
	{
		assert(m_LogicFramework != nullptr);

		auto CurrentWorldSeconds = GetWorldSeconds();
		auto SecondsBetweenFrames = CurrentWorldSeconds - LastLogicWorldTickSeconds;
		auto FixedLogicDeltaSeconds = 1 / static_cast<float>(m_LogicFramework->GetLogicFrameRates());
		if (SecondsBetweenFrames >= FixedLogicDeltaSeconds)
		{
			//std::cout << " Logic tick difference : " << SecondsBetweenFrames << std::endl;

			LastLogicWorldTickSeconds = CurrentWorldSeconds - SecondsBetweenFrames + FixedLogicDeltaSeconds;

			m_LogicFramework->LogicTick(FixedLogicDeltaSeconds);
		}
	}

	void World::Tick()
	{
		// Rendering tick, default frame rates is 60 FPS
		RenderingTick();

		// Physics tick
		PhysicsTick();

		// Logic tick, default frame rates is 30 FPS
		LogicFrameworkTick();
	}

	void World::PhysicsTick() noexcept
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


