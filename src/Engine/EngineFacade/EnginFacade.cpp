#include "EngineFacade.h"

#ifdef WIN32

#include "..\Graphics\SceneManager.h"

#else

#include "../Graphics/SceneManager.h"

#endif // WIN32

namespace cxc {

	static double _x=0.0f, _y=0.0f;

	std::function<void(int key, int scancode, int action, int mods)>
		EngineFacade::KeyInputCallBack = [=](int,int,int,int) {};

	void KeyBoradCallBack(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		EngineFacade::KeyInputCallBack(key,scancode,action,mods);
	}

	void CursorPosCallBack(GLFWwindow *window, double x, double y)
	{
		auto pEngine = EngineFacade::GetInstance();
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
		auto pEngine = EngineFacade::GetInstance();
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
		auto pEngine = EngineFacade::GetInstance();
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

	// Multi-threading rendering
	auto RenderingFunc = [&]() {

		auto pEngine = EngineFacade::GetInstance();
		auto m_pWindowMgr = pEngine->m_pWindowMgr;
		auto m_pSceneMgr = pEngine->m_pSceneMgr;

		std::unique_lock<std::mutex> lk(pEngine->m_InitLock);

		pEngine->m_pWindowMgr->InitGL();

		// Create window
		if (!pEngine->CreateAndDisplayWindow(m_pWindowMgr->GetWindowWidth(), m_pWindowMgr->GetWindowHeight(), m_pWindowMgr->GetWindowTitle()))
		{

			// shutdown and clean
			glfwTerminate();

			lk.unlock();
            pEngine->cv.notify_one();
            pEngine->m_RunLock.lock();

			return;
		}
		
		if (pEngine->Renders.empty())
		{
			// shutdown and clean
			glfwTerminate();

			lk.unlock();
			pEngine->cv.notify_one();
			pEngine->m_RunLock.lock();

			return;
		}
		else
		{
			while (!pEngine->Renders.empty())
			{
				auto pair = pEngine->Renders.back();
				pEngine->Renders.pop_back();

				if (pair.second->CreateShaderProgram())
				{
					m_pSceneMgr->m_pRendererMgr->addRender(pair.first,pair.second);
				}
				else
				{
					// shutdown and clean
					glfwTerminate();

					lk.unlock();
					pEngine->cv.notify_one();
					pEngine->m_RunLock.lock();

					return;
				}
			}
		}

		// Init input mode
		pEngine->InitEngine();

		auto pShadowRender = dynamic_cast<ShadowMapRender*>
			(pEngine->m_pSceneMgr->m_pRendererMgr->GetRenderPtr("ShadowRender"));
		if (pShadowRender)
			pShadowRender->InitShadowMapRender();

		// Init camera params and set input callback func
		m_pSceneMgr->InitCameraStatus(m_pWindowMgr->GetWindowHandle());

		// Init physics engine
		dInitODE(); pEngine->ODE_initialized = true;
		m_pSceneMgr->CreatePhysicalWorld(pEngine->m_Gravity);

		pEngine->EngineInitilized = true;

		lk.unlock();
		pEngine->cv.notify_one();
		pEngine->m_RunLock.lock();

		m_pSceneMgr->initResources();

		// Construct Octree
		//m_pSceneMgr->BuildOctree();

		// Begin event looping
		pEngine->GameLooping();

		// release buffers resources
		m_pSceneMgr->releaseBuffers();

		// Shutdown GL context
		pEngine->CleanGL();

		// release the lock
		pEngine->m_RunLock.unlock();

	};

	EngineFacade::EngineFacade()
		:
		GameOver(GL_FALSE),
		m_Gravity(glm::vec3({0,-9.81f,0})),
		ODE_initialized(false),pause(false),MultiThreading(false),
		EngineInitilized(false)
	{

		m_pInputMgr = InputManager::GetInstance();
		m_pWindowMgr = WindowManager::GetInstance();
		 m_pSceneMgr = SceneManager::GetInstance();
	}

	EngineFacade::~EngineFacade()
	{
		if(ODE_initialized)
			// Deallocation for extra memory of ODE runtime
			dCloseODE();
	}

	void EngineFacade::addObject(const std::shared_ptr<Object3D > &pObject, bool isKinematics) noexcept
	{
		if(pObject)
			m_pSceneMgr->AddObject(pObject->GetObjectName(),pObject,isKinematics);
	}

	GLboolean EngineFacade::CreateAndDisplayWindow(GLint Width,
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

	void EngineFacade::addShader(const std::string &name, BaseRender *Render)
	{
		Renders.emplace_back(std::make_pair(name,Render));
	}

	void EngineFacade::SetGravity(GLfloat x, GLfloat y, GLfloat z) noexcept
	{
		m_Gravity = glm::vec3({ x,y,z });

		if(m_pSceneMgr->m_WorldID)
			dWorldSetGravity(m_pSceneMgr->m_WorldID, x, y, z);
	}

	void EngineFacade::SetWindowParams(const WindowDescriptor &windes) noexcept
	{
		m_pWindowMgr->SetEnableDepthFlag(windes.isEnableDepth);
		m_pWindowMgr->SetForwardCompatibleFlag(windes.isForwardComptible);
		m_pWindowMgr->SetSamplingLevel(windes.SamplingLevel);
		m_pWindowMgr->SetBackGroundColor(windes.BackGroundColor.Red,
											windes.BackGroundColor.Green,
											windes.BackGroundColor.Blue,
											windes.BackGroundColor.Alpha);
	}

	void EngineFacade::waitForStop() noexcept
	{
		if (m_RenderingThread != nullptr) {
			// Join the rendering thread
			m_RenderingThread->join();

			// Reset to nullptr
			m_RenderingThread.reset();
		}
	}

	void EngineFacade::Init() noexcept
	{
		if (MultiThreading)
		{
			m_RunLock.lock();
			m_RenderingThread = std::make_unique<std::thread>(RenderingFunc);

			std::unique_lock<std::mutex> lk(m_InitLock);
			cv.wait(lk, [=] {return EngineInitilized; });
		}
		else
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
				while(!Renders.empty())
				{
					auto pair = Renders.back();
					Renders.pop_back();

					if (pair.second->CreateShaderProgram())
					{
						m_pSceneMgr->m_pRendererMgr->addRender(pair.first,pair.second);
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

			// Init camera params and set input callback func
			m_pSceneMgr->InitCameraStatus(m_pWindowMgr->GetWindowHandle());

            //Init physics engine
			dInitODE();ODE_initialized = true;
			m_pSceneMgr->CreatePhysicalWorld(m_Gravity);
		}
	}

	void EngineFacade::run() noexcept
	{
		if (MultiThreading) {
			m_RunLock.unlock();
		}
		else
		{

			m_pSceneMgr->initResources();

			// Contruct octree
			// m_pSceneMgr->BuildOctree();

			// Begin event looping
			GameLooping();

			// release buffers resources
			m_pSceneMgr->releaseBuffers();

			// Shutdown GL context
			CleanGL();
		}

	}

	void EngineFacade::SetGraphicsLibVersion(GLint HighByte, GLint LowByte) noexcept
	{
		m_pWindowMgr->SetDriverVersion(HighByte,LowByte);
	}

	void EngineFacade::InitEngine() noexcept
	{

		glEnable(GL_CULL_FACE);
		m_pInputMgr->SetInputModel(m_pWindowMgr->GetWindowHandle());

	}

	void EngineFacade::CleanFrameBuffer() const noexcept
	{
		if (m_pWindowMgr->isEnableDepthTest())
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		else
			glClear(GL_COLOR_BUFFER_BIT);
	}

	void EngineFacade::StoreAndSetMousePos() noexcept
	{
		m_pInputMgr->UpdateMousePos(m_pWindowMgr->GetWindowHandle());
		m_pInputMgr->SetMouseScreenPos(m_pWindowMgr->GetWindowHandle(), m_pWindowMgr->GetWindowWidth() / 2, m_pWindowMgr->GetWindowHeight() / 2);
	}

	void EngineFacade::SetBackGroundColor(float red, float green, float blue, float alpha) noexcept
	{
		m_pWindowMgr->SetBackGroundColor(red,green,blue,alpha);
	}

	void EngineFacade::RenderingScenes() noexcept
	{
		m_pSceneMgr->TickScene();
	}

	void EngineFacade::ProcessingPhysics() noexcept
	{

		m_pSceneMgr->UpdateMeshTransMatrix();

		dSpaceCollide(m_pSceneMgr->m_TopLevelSpace,reinterpret_cast<void *>(m_pSceneMgr.get()),&SceneManager::nearCallback);

		dWorldStep(m_pSceneMgr->m_WorldID, WOLRD_QUICK_STEPSIZE);

		dJointGroupEmpty(m_pSceneMgr->m_ContactJoints);

	}

	void EngineFacade::GameLooping() noexcept
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

			/* Disable the physics for testing purposes
			if(!pause)
				// Processing physical status
				ProcessingPhysics();
			*/

			// Rendering scenes
			RenderingScenes();

			// Swap front and back buffer
			glfwSwapBuffers(WindowHandler);

			glfwPollEvents();

		} while (!GameOver && glfwWindowShouldClose(WindowHandler) == 0);

		GameOver = true;

	}
}


