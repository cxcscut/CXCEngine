#include "EngineFacade.h"
#include "..\Graphics\SceneManager.h"

namespace cxc {

	static double _x=0.0f, _y=0.0f;

	void KeyBoradCallBack(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		
	}

	void CursorPosCallBack(GLFWwindow *window, double x, double y)
	{
		auto pEngine = EngineFacade::GetInstance();
		auto ProgramID = pEngine->m_pSceneMgr->m_pRendererMgr->GetShaderProgramID(CXC_SPRITE_SHADER_PROGRAM);
		auto wHandle = pEngine->m_pWindowMgr->GetWindowHandle();

		double dx = x-_x, dy = y-_y;
		_x = x; _y = y;

		double theta_xoz = -PI * (dx / pEngine->m_pWindowMgr->GetWindowWidth() / 2);
		double theta_y = -PI * (dy / pEngine->m_pWindowMgr->GetWindowHeight() / 2);

		// Update angles
		pEngine->m_pSceneMgr->m_pCamera->ComputeAngles();

		// Rotate Camera 
		pEngine->m_pSceneMgr->m_pCamera->theta_xoz += theta_xoz;
		pEngine->m_pSceneMgr->m_pCamera->theta_y += theta_y;

		if ((pEngine->m_pSceneMgr->m_pCamera->theta_y > PI - 0.1f) || (pEngine->m_pSceneMgr->m_pCamera->theta_y < 0.1f))
			pEngine->m_pSceneMgr->m_pCamera->theta_y = pEngine->m_pSceneMgr->m_pCamera->theta_y < 0.1f ? 0.1f : PI - 0.1f;

		pEngine->m_pSceneMgr->m_pCamera->ComputePosition();
		pEngine->m_pSceneMgr->m_pCamera->ComputeViewMatrix();
		pEngine->m_pSceneMgr->m_pCamera->BindViewMatrix(ProgramID);

		//m_RM->SetLightPos(camera->eye_pos);
		//m_RM->BindLightingUniforms();

		// Rotate object 
		/* 
		// Rotate left and right
		Engine->m_SceneManager->RotateAllSprites(glm::vec3(0,1,0),theta_xoz); 

		// Rotate up and down
		Engine->m_SceneManager->RotateAllSprites(glm::vec3(1,0,0),theta_y);
		Engine->m_SceneManager->RotateAllSprites(glm::vec3(0,0,1), theta_y);
		*/
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
		auto CurrentProgramID = pEngine->m_pSceneMgr->m_pRendererMgr->GetShaderProgramID(CXC_SPRITE_SHADER_PROGRAM);
		auto camera = pEngine->m_pSceneMgr->m_pCamera;
		if (!wHandle) return;

		glm::vec3 eye_direction = glm::normalize(camera->origin - camera->eye_pos);
		float distance = glm::length(camera->eye_pos - camera->origin);

		// y > 0 when srcoll up , y < 0 when scroll down 
		if (y>0) 
		{
			// srcoll up
			if (distance > MIN_DISTANCE) {
				camera->eye_pos += ZOOMING_SPEED * eye_direction;
			}
		}
		else
		{
			
			// scroll down
			if (distance <= MAX_DISTANCE) {
				camera->eye_pos -= ZOOMING_SPEED  * eye_direction;
			}
			
		}

		camera->ComputeViewMatrix();
		camera->BindViewMatrix(CurrentProgramID);

		//Engine->GetRendermanagerPtr()->SetLightPos(camera->eye_pos);
		//Engine->GetRendermanagerPtr()->BindLightingUniforms();

	}

	EngineFacade::EngineFacade()
		:
		GameOver(GL_FALSE), VertexShaderPath(".\\Engine\\Shader\\StandardVertexShader.glsl"),
		FragmentShaderPath(".\\Engine\\Shader\\StandardFragmentShader.glsl")
	{

		m_pInputMgr = InputManager::GetInstance();
		m_pWindowMgr = WindowManager::GetInstance();
		 m_pSceneMgr = SceneManager::GetInstance();
	}

	EngineFacade::~EngineFacade()
	{

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

		if (glewInit() != GLEW_OK)
		{
			return GL_FALSE;
		}

		return GL_TRUE;
	}

	GLboolean EngineFacade::LoadShader(ShaderType Type,
									const std::string &vertex_shader_path,
									const std::string &fragment_shader_path)
	{
		
		if (!m_pSceneMgr->m_pRendererMgr->isShaderLoaded(Type)) {
			ProgramStruct m_ProgramStruct;
			if (!m_pSceneMgr->m_pRendererMgr->CreateShaderProgram(m_ProgramStruct, vertex_shader_path, fragment_shader_path))
				return GL_FALSE;
			m_pSceneMgr->m_pRendererMgr->CreateProgram(Type,m_ProgramStruct);

			return GL_TRUE;
		}
		else
			m_pSceneMgr->m_pRendererMgr->BindShaderWithExistingProgram(Type,vertex_shader_path,fragment_shader_path);

		return GL_TRUE;
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
	
	void EngineFacade::run() noexcept
	{
		// Multi-threading rendering
		auto RenderingFunc = [&]() {

			// Create window
			if (!CreateAndDisplayWindow(m_pWindowMgr->GetWindowWidth(), m_pWindowMgr->GetWindowHeight(), m_pWindowMgr->GetWindowTitle()))
			{

				// release program resources
				m_pSceneMgr->m_pRendererMgr->releaseResources();

				// shutdown and clean
				glfwTerminate();
				return;
			}

			if (!LoadShader(CXC_SPRITE_SHADER_PROGRAM, VertexShaderPath, FragmentShaderPath))
			{
				// release program resources
				m_pSceneMgr->m_pRendererMgr->releaseResources();

				// shutdown and clean
				glfwTerminate();
				return;
			}

			// Init input mode
			InitEngine();

			// Init camera params and set input callback func
			m_pSceneMgr->InitCameraStatus(m_pWindowMgr->GetWindowHandle());

			// Load texture
			m_pSceneMgr->m_pTextureMgr->LoadAllTexture();

			// Accuire resources
			m_pSceneMgr->initResources();

			// Begin event looping
			GameLooping();

			// release buffers resources
			m_pSceneMgr->releaseBuffers();

			// release program resources
			m_pSceneMgr->m_pRendererMgr->releaseResources();

			// Release texture resources
			m_pSceneMgr->m_pTextureMgr->RemoveAllTexture();

			// Shutdown GL context
			CleanGL();

		};

		// Begin rendering thread, it will be joined when waitForStop() invoked
		m_RenderingThread = std::make_unique<std::thread>(RenderingFunc);

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

	void EngineFacade::ActivateRenderer(ShaderType Type) const noexcept
	{
		auto id = m_pSceneMgr->m_pRendererMgr->GetShaderProgramID(Type);
		if(id)
			glUseProgram(id);
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

	void EngineFacade::RenderingScenes() const noexcept
	{
		ActivateRenderer(CXC_SPRITE_SHADER_PROGRAM);
		m_pSceneMgr->DrawScene();
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
				m_pSceneMgr->UpdateCameraPos(m_pWindowMgr->GetWindowHandle(),m_pInputMgr->GetXPos(),m_pInputMgr->GetYPos(),
					m_pWindowMgr->GetWindowHeight(),m_pWindowMgr->GetWindowWidth());
			}

			// Rendering scenes
			RenderingScenes();

			// Swap front and back buffer
			glfwSwapBuffers(WindowHandler);

			glfwPollEvents();

		} while (!GameOver && glfwWindowShouldClose(WindowHandler) == 0);

		GameOver = true;
	}
}


