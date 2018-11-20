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
		auto pRender = pEngine->m_pSceneMgr->m_pRendererMgr->GetRenderPtr("StandardRender");
		if (!pRender) return;

		auto ProgramID = pRender->GetProgramID();
		auto wHandle = pEngine->m_pWindowMgr->GetWindowHandle();

		auto camera = pEngine->m_pSceneMgr->m_pCamera;

		double dx = x-_x, dy = y-_y;
		_x = x; _y = y;

		double DeltaThetaXOY = -PI * (dx / pEngine->m_pWindowMgr->GetWindowWidth() / 2);
		double DeltaThetaToXOY = PI * (dy / pEngine->m_pWindowMgr->GetWindowHeight() / 2);

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
		auto wHandle = pEngine->m_pWindowMgr->GetWindowHandle();
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
		auto wHandle = pEngine->m_pWindowMgr->GetWindowHandle();
		auto pRender = pEngine->m_pSceneMgr->m_pRendererMgr->GetRenderPtr("StandardRender");
		if (!pRender) return;

		auto CurrentProgramID = pRender->GetProgramID();
		auto camera = pEngine->m_pSceneMgr->m_pCamera;
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

		m_pInputMgr = InputManager::GetInstance();
		m_pWindowMgr = WindowManager::GetInstance();
		m_pSceneMgr = SceneManager::GetInstance();
	}

	World::~World()
	{

	}

	void World::AddObject(const std::shared_ptr<Object3D > &pObject, bool isKinematics) noexcept
	{
		if (pObject)
		{
			m_pSceneMgr->AddObjectInternal(pObject->GetObjectName(), pObject, isKinematics);

			pObject->InitializeRigidBody(m_PhysicalWorld->GetWorldID(), m_PhysicalWorld->GetTopSpaceID());
		}
	}

	GLboolean World::CreateAndDisplayWindow(GLint Width,
												GLint Height,
												const std::string Title)
	{
		m_pWindowMgr->SetWindowHeight(Height);
		m_pWindowMgr->SetWindowWidth(Width);
		m_pWindowMgr->SetWindowTitle(Title);

		if (!m_pWindowMgr->PrepareResourcesForCreating())
			return GL_FALSE;

		if (!m_pWindowMgr->_CreateWindow()){
			glfwTerminate();
			return GL_FALSE;
		}

		m_pWindowMgr->InitContext();

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			return GL_FALSE;
		}

		return GL_TRUE;
	}

	void World::addShader(const std::string &name, BaseRender *Render)
	{
		Renders.emplace_back(std::make_pair(name,Render));
	}

	void World::SetGravity(GLfloat x, GLfloat y, GLfloat z) noexcept
	{
		auto InGravity = glm::vec3({ x,y,z });

		assert(m_PhysicalWorld != nullptr);
		m_PhysicalWorld->SetGravity(InGravity);
	}

	void World::SetWindowParams(const WindowDescriptor &windes) noexcept
	{
		m_pWindowMgr->SetEnableDepthFlag(windes.isEnableDepth);
		m_pWindowMgr->SetForwardCompatibleFlag(windes.isForwardComptible);
		m_pWindowMgr->SetSamplingLevel(windes.SamplingLevel);
		m_pWindowMgr->SetBackGroundColor(windes.BackGroundColor.Red,
											windes.BackGroundColor.Green,
											windes.BackGroundColor.Blue,
											windes.BackGroundColor.Alpha);
	}

	void World::Init() noexcept
	{
		m_pWindowMgr->InitGL();

		// Create window
		if (!CreateAndDisplayWindow(m_pWindowMgr->GetWindowWidth(), m_pWindowMgr->GetWindowHeight(), m_pWindowMgr->GetWindowTitle()))
		{

			// shutdown and clean
			glfwTerminate();
			return;
		}

		if (Renders.empty())
			return;
		else
		{
			while (!Renders.empty())
			{
				auto pair = Renders.back();
				Renders.pop_back();

				if (pair.second->CreateShaderProgram())
				{
					m_pSceneMgr->m_pRendererMgr->addRender(pair.first, pair.second);
				}
				else
				{
					glfwTerminate();
					return;
				}
			}
		}

		// Init input mode
		InitEngine();

		auto pShadowRender = dynamic_cast<ShadowMapRender*>
			(m_pSceneMgr->m_pRendererMgr->GetRenderPtr("ShadowRender"));
		if (pShadowRender)
			pShadowRender->InitShadowMapRender();

		// Turn off the vsync
		//glfwSwapInterval(0);

		// Init camera params and set input callback func
		m_pSceneMgr->InitCameraStatus(m_pWindowMgr->GetWindowHandle());

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
		// m_pSceneMgr->BuildOctree();

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

	void World::SetGraphicsLibVersion(GLint HighByte, GLint LowByte) noexcept
	{
		m_pWindowMgr->SetDriverVersion(HighByte,LowByte);
	}

	void World::InitEngine() noexcept
	{
		glEnable(GL_CULL_FACE);
		m_pInputMgr->SetInputModel(m_pWindowMgr->GetWindowHandle());
	}

	void World::CleanFrameBuffer() const noexcept
	{
		if (m_pWindowMgr->isEnableDepthTest())
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else
			glClear(GL_COLOR_BUFFER_BIT);
	}

	void World::StoreAndSetMousePos() noexcept
	{
		m_pInputMgr->UpdateMousePos(m_pWindowMgr->GetWindowHandle());
		m_pInputMgr->SetMouseScreenPos(m_pWindowMgr->GetWindowHandle(), m_pWindowMgr->GetWindowWidth() / 2, m_pWindowMgr->GetWindowHeight() / 2);
	}

	void World::SetBackGroundColor(float red, float green, float blue, float alpha) noexcept
	{
		m_pWindowMgr->SetBackGroundColor(red,green,blue,alpha);
	}

	void World::RenderingTick()
	{
		assert(m_pSceneMgr != nullptr);

		auto CurrentWorldSeconds = GetWorldSeconds();
		auto SecondsBetweenFrames = CurrentWorldSeconds - LastRenderingTickSeconds;
		auto FixedRenderingDeltaSeconds = 1 / static_cast<float>(RenderingFrameRates);
		if (SecondsBetweenFrames >= FixedRenderingDeltaSeconds)
		{
			//std::cout << "Rendering tick difference : " << SecondsBetweenFrames << std::endl;

			LastRenderingTickSeconds = CurrentWorldSeconds - SecondsBetweenFrames + FixedRenderingDeltaSeconds;

			m_pSceneMgr->Tick(SecondsBetweenFrames);
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
		auto WindowHandler = m_pWindowMgr->GetWindowHandle();
		do {

			// clean frame buffer for rendering
			CleanFrameBuffer();

			if (m_pSceneMgr->m_pCamera->m_CameraMode == CAMERA_FREE)
			{
				StoreAndSetMousePos();
 				m_pSceneMgr->UpdateCameraPos(m_pWindowMgr->GetWindowHandle(), m_pInputMgr->GetXPos(), m_pInputMgr->GetYPos(),
					m_pWindowMgr->GetWindowHeight(), m_pWindowMgr->GetWindowWidth());
			}

			// World tick
			Tick();

			// Swap front and back buffer
			glfwSwapBuffers(WindowHandler);

			glfwPollEvents();

		} while (!GameOver && glfwWindowShouldClose(WindowHandler) == 0);

		GameOver = true;

	}
}


