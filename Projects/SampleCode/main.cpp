#include "CXCEngine.h"
#include "Rendering/DeferredRenderer.h"
#include "Rendering/DeferredRenderPipeline.h"
#include "Rendering/ForwardRenderer.h"
#include "Rendering/ShadowRenderer.h"
#include "Rendering/ShadowRenderPipeline.h"
#include "Utilities/DebugLogger.h"
#include "Utilities/DebugDrawHelper.h"
#include "Scene/SceneContext.h"
#include "Actor/CLightActor.h"
#include "Actor/CCameraActor.h"

using namespace cxc;

// Forward render shaders
static const std::string ForwardRenderVSPath = "G:\\cxcengine\\Shader\\ForwardRendering\\ForwardRenderVS.glsl";
static const std::string ForwardRenderFSPath = "G:\\cxcengine\\Shader\\ForwardRendering\\ForwardRenderFS.glsl";
static const std::string DeferredRenderVSPath = "G:\\cxcengine\\Shader\\DeferredRendering\\DeferredRenderVS.glsl";
static const std::string DeferredRenderFSPath = "G:\\cxcengine\\ShaderDeferredRendering\\DeferredRenderFS.glsl";
static const std::string ShadowRenderDepthVSPath = "G:\\cxcengine\\Shader\\ShadowRendering\\depthTextureVS.glsl";
static const std::string ShadowRenderDepthFSPath = "G:\\cxcengine\\Shader\\ShadowRendering\\depthTextureFS.glsl";
static const std::string ShadowRenderVSPath = "G:\\cxcengine\\Shader\\ShadowRendering\\ShadowVS.glsl";
static const std::string ShadowRenderFSPath = "G:\\cxcengine\\Shader\\ShadowRendering\\ShadowFS.glsl";

static const std::string SceneFBXFile = "G:\\cxcengine\\Projects\\Models\\EN_Building_H_03.FBX";
static const std::string HumanoidScene = "G:\\cxcengine\\Projects\\Models\\humanoid.fbx";

std::shared_ptr<SubMeshRenderer> CreateDeferredRenderer();
std::shared_ptr<SubMeshRenderer> CreateForwardRenderer();
std::shared_ptr<SubMeshRenderer> CreateShadowRenderer();
void BindSubMeshRenderer(std::shared_ptr<SubMeshRenderer> pRenderer, const std::vector<std::shared_ptr<CActor>>& Objects);
std::vector<std::shared_ptr<CActor>> CreateActors(std::shared_ptr<SceneContext> Context);

int main()
{
	glm::vec3 CameraPos = glm::vec3(50, 50, 50);
	glm::vec3 CameraOrigin = glm::vec3(0, 0, 0);
	glm::vec3 CameraUpVector = glm::vec3(0, 0, 1); 
	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 10000.0f);

	DisplayParameters DisplayConf;
	DisplayConf.ApplicationTitle = "Powered by CXCEngine";
	DisplayConf.bIsDecorated = true;
	DisplayConf.WindowHeight = 600;
	DisplayConf.WindowWidth = 800;
	DisplayConf.WindowPosX = 200;
	DisplayConf.WindowPosY = 200;
	GEngine::ConfigureEngineDisplaySettings(DisplayConf);
	GEngine::InitializeEngine();

	auto pWorld = World::GetInstance();
	auto pRendererMgr = RendererManager::GetInstance();
	auto pSceneManager = SceneManager::GetInstance();
	auto pRenderer = CreateForwardRenderer();
	pRendererMgr->AddRenderer(pRenderer);

	auto pCamera = NewObject<Camera>();
	pCamera->CameraName = "MainCamera";
	pCamera->EyePosition = CameraPos;
	pCamera->CameraOrigin = CameraOrigin;
	pCamera->UpVector = CameraUpVector;
	pCamera->SetAllMatrix(glm::lookAt(CameraPos, CameraOrigin, CameraUpVector), ProjectionMatrix);
	pCamera->ComputeAngles();
	pCamera->ComputeViewMatrix();
	auto pCameraActor = NewObject<CCameraActor>();
	pCameraActor->SetCamera(pCamera);
	pWorld->AddActor(pCameraActor);

	pSceneManager->SetCameraActive(pSceneManager->GetCamera(0));

	auto SceneContextCache = NewObject<SceneContext>();
	bool bResult = pWorld->LoadSceneFromFBX(SceneFBXFile, SceneContextCache);
	auto Actors = CreateActors(SceneContextCache);
	if (bResult)
	{
		BindSubMeshRenderer(pRenderer, Actors);
	}
	else
	{
		DEBUG_LOG(eLogType::Error, "Failed to load scene : " + SceneFBXFile + '\n');
	}

	for (auto Actor : Actors)
	{
		pWorld->AddActor(Actor);
	}

	DrawDebugCube(glm::vec3(0,0,50), glm::vec3(5,5,5), glm::vec3(1,0,0));

	// Start engine
	GEngine::StartEngine();

	return 0;
}

std::vector<std::shared_ptr<CActor>> CreateActors(std::shared_ptr<SceneContext> Context)
{
	auto pSceneManager = SceneManager::GetInstance();
	std::vector<std::shared_ptr<CActor>> RetActors;

	// Create actor
	for (auto pMesh : Context->Meshes)
	{
		if (pMesh)
		{
			auto Actor = NewObject<CActor>(pMesh->GetMeshName());
			auto StaticMeshComponent = NewObject<CStaticMeshComponent>(pMesh);
			Actor->AttachComponent(StaticMeshComponent);
			RetActors.push_back(Actor);
		}
	}

	// Create light actor
	for (auto pLight : Context->Lights)
	{
		if (pLight)
		{
			auto LightActor = NewObject<CLightActor>(pLight->GetLightName());
			LightActor->SetLight(pLight);
			RetActors.push_back(LightActor);
		}
	}

	// Create camera actor
	for (auto pCamera : Context->Cameras)
	{
		if (pCamera)
		{
			auto CameraActor = NewObject<CCameraActor>(pCamera->CameraName);
			CameraActor->SetCamera(pCamera);
			RetActors.push_back(CameraActor);
		}
	}

	return RetActors;
}

void BindSubMeshRenderer(std::shared_ptr<SubMeshRenderer> pRenderer, const std::vector<std::shared_ptr<CActor>>& Objects)
{
	auto pRendererMgr = RendererManager::GetInstance();

	for (auto Object : Objects)
	{
		auto ComponentCount = Object->GetComponentCount();
		for (size_t Index = 0; Index < ComponentCount; ++Index)
		{
			auto StaticMeshComponent = std::dynamic_pointer_cast<CStaticMeshComponent>(Object->GetComponent(Index));
			if (StaticMeshComponent != nullptr)
			{
				uint32_t SubMeshCount = StaticMeshComponent->GetStaticMesh()->GetSubMeshCount();
				for (uint32_t Index = 0; Index < SubMeshCount; ++Index)
				{
					auto pSubMesh = StaticMeshComponent->GetStaticMesh()->GetSubMesh(Index);
					pRendererMgr->BindSubMeshRenderer(pSubMesh, pRenderer);
				}
			}
		}
	}
}

std::shared_ptr<SubMeshRenderer> CreateForwardRenderer()
{
	auto pRendererMgr = RendererManager::GetInstance();

	// Foward Phong render with no shadows
	auto ForwardPhongVS = pRendererMgr->FactoryShader("ForwardVS", eShaderType::VERTEX_SHADER, ForwardRenderVSPath);
	auto ForwardPhongFS = pRendererMgr->FactoryShader("ForwardFS", eShaderType::FRAGMENT_SHADER, ForwardRenderFSPath);

	auto ForwardPhongPipeline = NewObject<ForwardRenderPipeline>("ForwardPhongPipeline");
	ForwardPhongPipeline->AttachShader(ForwardPhongVS);
	ForwardPhongPipeline->AttachShader(ForwardPhongFS);

	auto PhongRender = NewObject<ForwardRenderer>("ForwardPhongRender");
	PhongRender->PushPipeline(ForwardPhongPipeline);
	PhongRender->InitializeRenderer();

	return PhongRender;
}

std::shared_ptr<SubMeshRenderer> CreateDeferredRenderer()
{
	auto pRendererMgr = RendererManager::GetInstance();

	auto pDeferredRenderer = NewObject<DeferredRenderer>("DeferredRenderer");
	auto pDeferredRenderPipeline = NewObject<DeferredRenderPipeline>();

	auto DeferredRenderVS = pRendererMgr->FactoryShader("DeferredShaderVS", eShaderType::VERTEX_SHADER, DeferredRenderVSPath);
	auto DeferredRenderFS = pRendererMgr->FactoryShader("DeferredShaderFS", eShaderType::FRAGMENT_SHADER, DeferredRenderFSPath);
	pDeferredRenderPipeline->AttachShader(DeferredRenderVS);
	pDeferredRenderPipeline->AttachShader(DeferredRenderFS);

	pDeferredRenderer->PushPipeline(pDeferredRenderPipeline);
	pDeferredRenderer->InitializeRenderer();

	return pDeferredRenderer;
}

std::shared_ptr<SubMeshRenderer> CreateShadowRenderer()
{
	auto pRendererMgr = RendererManager::GetInstance();

	auto ShadowMapRender = NewObject<ShadowRenderer>("ShadowRenderer");
	auto ShadowBasePassPipeline = NewObject<ShadowRenderBasePassPipeline>();
	auto ShadowLightingPassPipeline = NewObject<ShadowRenderLightingPassPipeline>();

	auto ShadowMapCookingVS = pRendererMgr->FactoryShader("ShadowMapCookingVS", eShaderType::VERTEX_SHADER, ShadowRenderDepthVSPath);
	auto ShadowMapCookingFS = pRendererMgr->FactoryShader("ShadowMapCookingFS", eShaderType::FRAGMENT_SHADER, ShadowRenderDepthFSPath);
	auto ShadowVS = pRendererMgr->FactoryShader("ShadowVS", eShaderType::VERTEX_SHADER, ShadowRenderVSPath);
	auto ShadowFS = pRendererMgr->FactoryShader("ShadowFS", eShaderType::FRAGMENT_SHADER, ShadowRenderFSPath);

	ShadowBasePassPipeline->AttachShader(ShadowMapCookingVS);
	ShadowBasePassPipeline->AttachShader(ShadowMapCookingFS);
	ShadowLightingPassPipeline->AttachShader(ShadowVS);
	ShadowLightingPassPipeline->AttachShader(ShadowFS);

	ShadowMapRender->PushPipeline(ShadowBasePassPipeline);
	ShadowMapRender->PushPipeline(ShadowLightingPassPipeline);

	ShadowMapRender->InitializeRenderer();
	ShadowMapRender->SetShadowMapResolution(512);

	return ShadowMapRender;
}