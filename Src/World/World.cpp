#include "World/World.h"
#include "Components/CStaticMeshComponent.h"
#include "Components/CLightComponent.h"
#include "Components/CCameraComponent.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneContext.h"
#include "Utilities/DebugLogger.h"

namespace cxc {

	auto LogicThreadFunc = [](bool GameOver, std::shared_ptr<World> pWorld)
	{
		while (!GameOver && pWorld)
		{
			pWorld->LogicTick();
		}
	};

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
			DEBUG_LOG(eLogType::Error, "GLEW initialization failed\n");
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
			DEBUG_LOG(eLogType::Error, "World::Initialize, Failed to create display window \n");

			// shutdown and clean
			glfwTerminate();
			return;
		}

		// Init input mode
		InitInputMode();

		// Turn on the vsync
		glfwSwapInterval(0);

		// Initialize physics engine
		m_PhysicalWorld->InitializePhysicalWorld();

		// Intialize logic world
		m_LogicFramework->IntializeLogicWorld();

		// Initialize RendererManager
		pSceneMgr->pRendererMgr->CreateEngineDefaultRenderer();
	}
 
	float World::GetWorldSeconds() const
	{
		auto CurrentSeconds = std::chrono::system_clock::now();
		auto Duration = std::chrono::duration_cast<std::chrono::microseconds>(CurrentSeconds - WorldStartSeconds);
		return  float(Duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
	}

	void World::StartToRun() noexcept
	{
		// Initialize the start time of the world
		WorldStartSeconds = std::chrono::system_clock::now();
		auto CurrentWorldSeconds = GetWorldSeconds();
		LastLogicWorldTickSeconds = CurrentWorldSeconds;
		LastRenderingTickSeconds = CurrentWorldSeconds;

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

	void World::RenderingTick(float DeltaSeconds)
	{
		// Clear the frame buffer
		CleanFrameBuffer();

		// Rendering the scene
		pSceneMgr->RenderingTick(DeltaSeconds);
	}

	void World::ProcessInput()
	{
		// Tick the InputManager
		InputManager::GetInstance()->HandleInput();
	}

	void World::LogicTick()
	{
		assert(m_LogicFramework != nullptr);

		auto CurrentWorldSeconds = GetWorldSeconds();
		auto SecondsBetweenFrames = CurrentWorldSeconds - LastLogicWorldTickSeconds;
		auto FixedLogicDeltaSeconds = 1 / static_cast<float>(m_LogicFramework->GetLogicFrameRates());

		if (SecondsBetweenFrames >= FixedLogicDeltaSeconds)
		{
			auto TickTimes = static_cast<uint32_t>(SecondsBetweenFrames / FixedLogicDeltaSeconds);

			m_LogicFramework->Tick(TickTimes * FixedLogicDeltaSeconds);
		}
		else
		{
			auto SleepMilliSeconds = static_cast<int>((FixedLogicDeltaSeconds - SecondsBetweenFrames) * 1000);

			// Waiting
			std::this_thread::sleep_for(std::chrono::milliseconds(SleepMilliSeconds));

			m_LogicFramework->Tick(FixedLogicDeltaSeconds);

			CurrentWorldSeconds = GetWorldSeconds();
		}

		LastLogicWorldTickSeconds = CurrentWorldSeconds;
	}

	void World::PhysicsTick(float DeltaSeconds)
	{
		m_PhysicalWorld->PhysicsTick(DeltaSeconds);
	}

	void World::AddActor(std::shared_ptr<CActor> Actor)
	{
		if (Actor)
		{
			// Add actor to the logic world
			m_LogicFramework->AddActor(Actor);

			// Add camera or light to the scene if has the component
			size_t ComponentCount = Actor->GetComponentCount();
			for (size_t Index = 0; Index < ComponentCount; ++Index)
			{
				// If the actor has CStatisMeshComponent, we add mesh to the scene for rendering
				auto StaticMeshComponent = std::dynamic_pointer_cast<CStaticMeshComponent>(Actor->GetComponent(Index));
				auto LightComponent = std::dynamic_pointer_cast<CLightComponent>(Actor->GetComponent(Index));
				auto CameraComponent = std::dynamic_pointer_cast<CCameraComponent>(Actor->GetComponent(Index));
				if (StaticMeshComponent)
				{
					// Add mesh to the scene 
					pSceneMgr->AddMesh(StaticMeshComponent->GetStaticMesh());
				}
				else if (LightComponent)
				{
					// If the actor has CLightComponent, we add the light to the scene
					pSceneMgr->AddLight(LightComponent->GetLight());
				}
				else if (CameraComponent)
				{
					// If the actor has CCameraComponent, we add the camera to the scene
					pSceneMgr->AddCamera(CameraComponent->GetCamera());
				}
			}
		}
	}

	void World::ProcessSceneNode(FbxNode* pRootNode, std::shared_ptr<SceneContext> OutSceneContext)  noexcept
	{
		if (!pRootNode || !OutSceneContext)
			return;

		if (pRootNode->GetNodeAttribute() != nullptr)
		{
			auto pPhysicalWorld = PhysicalWorld::GetInstance();
			FbxNodeAttribute::EType AttributeType;
			AttributeType = pRootNode->GetNodeAttribute()->GetAttributeType();

			switch (AttributeType)
			{
			default:
				break;

			case FbxNodeAttribute::eMesh:
			{
				std::vector<std::shared_ptr<Mesh>> LoadedMeshes;
				FbxAMatrix lGlobalPosition;
				bool bMeshLoadingRes = FBXSDKUtil::GetMeshFromNode(pRootNode, LoadedMeshes, pPhysicalWorld->GetWorldID(), pPhysicalWorld->GetTopSpaceID(), lGlobalPosition);
				if (!bMeshLoadingRes)
				{
					DEBUG_LOG(eLogType::Verbose, "SceneManager::ProcessSceneNode, Failed to load the mesh \n");
				}
				else
				{
					for (auto pMeshes : LoadedMeshes)
					{
						OutSceneContext->Meshes.push_back(pMeshes);
					}
				}
				break;
			}

			case FbxNodeAttribute::eLight:
			{
				std::vector<std::shared_ptr<LightSource>> LoadedLights;
				FbxAMatrix lGlobalPosition;
				bool bLightLoadingRes = FBXSDKUtil::GetLightFromRootNode(pRootNode, LoadedLights, lGlobalPosition);
				if (!bLightLoadingRes)
				{
					DEBUG_LOG(eLogType::Verbose, "SceneManager::ProcessSceneNode, Failed to load the lights \n");
				}
				else
				{
					for (auto pNewLight : LoadedLights)
					{
						OutSceneContext->Lights.push_back(pNewLight);
					}
				}

				break;
			}

			}
		}

		// Process the child node
		int ChildNodeCount = pRootNode->GetChildCount();
		for (int i = 0; i < ChildNodeCount; ++i)
		{
			ProcessSceneNode(pRootNode->GetChild(i), OutSceneContext);
		}
	}

	bool World::LoadSceneFromFBX(const std::string& filepath, std::shared_ptr<SceneContext> OutSceneContext) noexcept
	{
		FbxManager* pSdkManager = nullptr;
		FbxScene* pScene = nullptr;
		bool bSuccessfullyLoadedScene = false;

		// Initialize the FBX SDK
		FBXSDKUtil::InitializeSDKObjects(pSdkManager, pScene);
		bSuccessfullyLoadedScene = FBXSDKUtil::LoadScene(pSdkManager, pScene, filepath.c_str());
		if (!bSuccessfullyLoadedScene)
		{
			FBXSDKUtil::DestroySDKObjects(pSdkManager, bSuccessfullyLoadedScene);
			return false;
		}

		// Clear cache of the context
		OutSceneContext->ClearCache();

		// Process node from the root node of the scene
		ProcessSceneNode(pScene->GetRootNode(), OutSceneContext);

		// Destroy all the objects created by the FBX SDK
		FBXSDKUtil::DestroySDKObjects(pSdkManager, bSuccessfullyLoadedScene);

		return true;
	}

	void World::RemoveActor(uint32_t GUID)
	{
		auto RemovingActor = m_LogicFramework->GetActor(GUID);
		if (RemovingActor)
			RemoveActor(RemovingActor);
	}

	void World::RemoveActor(const std::string& ActorName)
	{
		auto RemovingActor = m_LogicFramework->GetActor(ActorName);
		if (RemovingActor)
			RemoveActor(RemovingActor);
	}

	void World::RemoveActor(std::shared_ptr<CActor> Actor)
	{
		if (Actor && m_LogicFramework->GetActor(Actor->GetName()) != nullptr)
		{
			// Remove the camera of light from the scene if has the components
			size_t ComponentCount = Actor->GetComponentCount();
			for (size_t Index = 0; Index < ComponentCount; ++Index)
			{
				auto CameraComponent = std::dynamic_pointer_cast<CCameraComponent>(Actor->GetComponent(Index));
				auto LightComponent = std::dynamic_pointer_cast<CLightComponent>(Actor->GetComponent(Index));
				auto StaticMeshComponent = std::dynamic_pointer_cast<CStaticMeshComponent>(Actor->GetComponent(Index));
				if (CameraComponent)
				{
					// Remove the camera if it has the CameraComponent
					pSceneMgr->RemoveCamera(CameraComponent->GetCamera());
				}
				else if (LightComponent)
				{
					// Remove the light if it has the LightComponent
					pSceneMgr->RemoveLight(LightComponent->GetLight());
				}
				else if (StaticMeshComponent)
				{
					// Remove the meshes if hast the StaticMeshComponent
					pSceneMgr->RemoveMesh(StaticMeshComponent->GetStaticMesh());
				}
			}

			// Remove actor from the logic world
			m_LogicFramework->RemoveActor(Actor);
		}
	}

	void World::Tick()
	{
		auto CurrentSeconds = GetWorldSeconds();
		auto RenderingTickDeltaSeconds = CurrentSeconds - LastRenderingTickSeconds;
		auto RenderingTickInterval = 1 / static_cast<float>(RenderingFrameRates);
		
		if (RenderingTickDeltaSeconds >= RenderingTickInterval)
		{
			auto TickTimes = static_cast<uint32_t>(RenderingTickDeltaSeconds / RenderingTickInterval);

			// Physics tick
			PhysicsTick(TickTimes * RenderingTickInterval);

			// Rendering tick
			RenderingTick(TickTimes * RenderingTickInterval);
		}
		else
		{
			// Sleep the thread
			auto SleepMilliSeconds = static_cast<uint32_t>((RenderingTickInterval - RenderingTickDeltaSeconds) * 1000);
			std::this_thread::sleep_for(std::chrono::milliseconds(SleepMilliSeconds));

			CurrentSeconds = GetWorldSeconds();

			// Physics tick
			PhysicsTick(RenderingTickInterval);

			// Rendering tick
			RenderingTick(RenderingTickInterval);
		}

		LastRenderingTickSeconds = CurrentSeconds;
	}

	void World::WorldLooping() noexcept
	{
		// Begin logic layer thread
		std::thread LogicThread(LogicThreadFunc, GameOver, shared_from_this());

		auto WindowHandler = pWindowMgr->GetWindowHandle();
		do {
			// Processing the input
			ProcessInput();

			// World tick
			Tick();

			// Swap front and back buffer
			glfwSwapBuffers(WindowHandler);

			glfwPollEvents();

		} while (!GameOver && glfwWindowShouldClose(WindowHandler) == 0);

		GameOver = true;

		LogicThread.join();
	}
}


