#include "EngineFacade.h"

namespace cxc {

	static double _x=0.0f, _y=0.0f;

	void KeyBoradCallBack(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		
	}

	void CursorPosCallBack(GLFWwindow *window, double x, double y)
	{
		auto Engine = EngineFacade::GetInstance();
		auto m_RM = Engine->GetRendermanagerPtr();
		auto m_WM = Engine->GetWindowMangaerPtr();
		auto camera = Engine->GetCameraPtr();

		auto ProgramID = m_RM->GetShaderProgramID(CXC_SPRITE_SHADER_PROGRAM);
		auto wHandle = m_WM->GetWindowHandle();

		double dx = x-_x, dy = y-_y;
		_x = x; _y = y;

		double theta_xoz = -PI * (dx / m_WM->GetWindowWidth() / 2);
		double theta_y = -PI * (dy / m_WM->GetWindowHeight() / 2);

		// Update angles
		camera->ComputeAngles();

		// Rotate Camera 
		camera->theta_xoz += theta_xoz;
		camera->theta_y += theta_y;

		if ((camera->theta_y > PI - 0.1f) || (camera->theta_y < 0.1f))
			camera->theta_y = camera->theta_y < 0.1f ? 0.1f : PI - 0.1f;

		camera->ComputePosition();
		camera->ComputeViewMatrix();
		camera->BindViewMatrix(ProgramID);

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
		auto Engine = EngineFacade::GetInstance();
		auto wHandle = Engine->GetWindowMangaerPtr()->GetWindowHandle();
		auto ProgramID = Engine->GetRendermanagerPtr()->GetShaderProgramID(CXC_SPRITE_SHADER_PROGRAM);
		auto CurrentProgramID = Engine->GetRendermanagerPtr()->GetShaderProgramID(CXC_SPRITE_SHADER_PROGRAM);
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
		auto Engine = EngineFacade::GetInstance();
		auto wHandle = Engine->GetWindowMangaerPtr()->GetWindowHandle();
		auto CurrentProgramID = Engine->GetRendermanagerPtr()->GetShaderProgramID(CXC_SPRITE_SHADER_PROGRAM);
		auto camera = Engine->GetCameraPtr();
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
		: m_pInputMgr(InputManager::GetInstance()),
		m_pRendererMgr(RendererManager::GetInstance()),
		m_pSceneMgr(SceneManager::GetInstance()),
		m_pWindowMgr(WindowManager::GetInstance()),
		m_pCamera(std::make_shared<Camera>()),
		GameOver(GL_FALSE), VertexShaderPath(".\\Engine\\Shader\\StandardVertexShader.glsl"),
		FragmentShaderPath(".\\Engine\\Shader\\StandardFragmentShader.glsl")
	{
	
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
		
		if (!m_pRendererMgr->isShaderLoaded(Type)) {
			ProgramStruct m_ProgramStruct;
			if (!m_pRendererMgr->CreateShaderProgram(m_ProgramStruct, vertex_shader_path, fragment_shader_path))
				return GL_FALSE;
			m_pRendererMgr->CreateProgram(Type,m_ProgramStruct);

			return GL_TRUE;
		}
		else
			m_pRendererMgr->BindShaderWithExistingProgram(Type,vertex_shader_path,fragment_shader_path);

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
				m_pRendererMgr->releaseResources();

				// shutdown and clean
				glfwTerminate();
				return;
			}

			if (!LoadShader(CXC_SPRITE_SHADER_PROGRAM, VertexShaderPath, FragmentShaderPath))
			{
				// release program resources
				m_pRendererMgr->releaseResources();

				// shutdown and clean
				glfwTerminate();
				return;
			}

			// Init input mode
			InitEngine();

			// Init camera params and set input callback func
			InitCameraStatus();

			// Load texture
			m_pSceneMgr->GetTextureManagerPtr()->LoadAllTexture();

			// Accuire resources
			m_pSceneMgr->initResources();

			// Begin event looping
			GameLooping();

			// release buffers resources
			m_pSceneMgr->releaseBuffers();

			// release program resources
			m_pRendererMgr->releaseResources();

			// Release texture resources
			m_pSceneMgr->GetTextureManagerPtr()->RemoveAllTexture();

			// Shutdown GL context
			CleanGL();

		};

		// Begin rendering thread, it will be joined when waitForStop() invoked
		m_RenderingThread = std::make_unique<std::thread>(RenderingFunc);

	}

	void EngineFacade::InitCameraStatus() noexcept
	{
		m_pCamera->InitLastTime();

		if (m_pCamera->m_CameraMode == CAMERA_FIXED)
		{
			//Set Keyboard and mouse callback function
			glfwSetKeyCallback(m_pWindowMgr->GetWindowHandle(), KeyBoradCallBack);
			glfwSetMouseButtonCallback(m_pWindowMgr->GetWindowHandle(), MouseCallBack);
			glfwSetScrollCallback(m_pWindowMgr->GetWindowHandle(), ScrollBarCallBack);
		}

		// Set Camera pos
		SetCameraParams(glm::vec3(0, 2000, 2000), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0),
			glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 10000.0f)
		);

	}

	void EngineFacade::SetLightPos(const glm::vec3 &light_pos) noexcept
	{
		m_pRendererMgr->SetLightPos(light_pos);
	}

	void EngineFacade::SetCameraMode(CameraModeType mode) noexcept
	{
		m_pCamera->m_CameraMode = mode;
	}

	void EngineFacade::SetGraphicsLibVersion(GLint HighByte, GLint LowByte) noexcept
	{
		m_pWindowMgr->SetDriverVersion(HighByte,LowByte);
	}

	void EngineFacade::InitEngine() noexcept
	{

		glDisable(GL_CULL_FACE);
		m_pInputMgr->SetInputModel(m_pWindowMgr->GetWindowHandle());

	}

	void EngineFacade::SetCameraParams(const glm::vec3 &eye, const glm::vec3 &origin, const glm::vec3 &up,
									const glm::mat4 &ProjectionMatrix) noexcept
	{

		m_pCamera->eye_pos = eye;
		m_pCamera->origin = origin;
		m_pCamera->up_vector = up;
		m_pCamera->SetAllMatrix(glm::lookAt(eye,origin,up), ProjectionMatrix);
		m_pCamera->ComputeAngles();
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
		auto id = m_pRendererMgr->GetShaderProgramID(Type);
		if(id)
			glUseProgram(id);
	}

	void EngineFacade::StoreAndSetMousePos() noexcept
	{
		m_pInputMgr->UpdateMousePos(m_pWindowMgr->GetWindowHandle());
		m_pInputMgr->SetMouseScreenPos(m_pWindowMgr->GetWindowHandle(), m_pWindowMgr->GetWindowWidth() / 2, m_pWindowMgr->GetWindowHeight() / 2);
	}

	void EngineFacade::UpdateCameraPos() noexcept
	{
			m_pCamera->ComputeMatrices_Moving(m_pWindowMgr->GetWindowHandle(), m_pInputMgr->GetXPos(),
			m_pInputMgr->GetYPos(), m_pWindowMgr->GetWindowHeight(), m_pWindowMgr->GetWindowWidth());

	}

	void EngineFacade::BindCameraUniforms() const noexcept
	{
		auto SpriteProgramID = m_pRendererMgr->GetShaderProgramID(CXC_SPRITE_SHADER_PROGRAM);
		m_pCamera->BindCameraUniforms(SpriteProgramID);
	}

	void EngineFacade::SetBackGroundColor(float red, float green, float blue, float alpha) noexcept
	{
		m_pWindowMgr->SetBackGroundColor(red,green,blue,alpha);
	}

	void EngineFacade::RenderingScenes() const noexcept
	{
		ActivateRenderer(CXC_SPRITE_SHADER_PROGRAM);
		GetSceneManagerPtr()->DrawScene();
	}	

	void EngineFacade::GameLooping() noexcept
	{

		auto WindowHandler = m_pWindowMgr->GetWindowHandle();
		do {

			// clean frame buffer for rendering
			CleanFrameBuffer();

			if (m_pCamera->m_CameraMode == CAMERA_FREE)
			{
				StoreAndSetMousePos();
				UpdateCameraPos();
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


