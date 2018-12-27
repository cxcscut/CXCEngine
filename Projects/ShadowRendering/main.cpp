#include "CXCEngine.h"
#include "Rendering/ForwardRenderer.h"
#include "Rendering/ShadowRenderer.h"
#include "Rendering/ShadowRenderPipeline.h"
#include "Rendering/DeferredRenderer.h"
#include "Rendering/DeferredRenderPipeline.h"

using namespace cxc;

static const std::string ShadowRenderDepthVSPath = "G:\\cxcengine\\Src\\GLSL\\ShadowRendering\\depthTextureVS.glsl";
static const std::string ShadowRenderDepthFSPath = "G:\\cxcengine\\Src\\GLSL\\ShadowRendering\\depthTextureFS.glsl";
static const std::string ShadowRenderVSPath = "G:\\cxcengine\\Src\\GLSL\\ShadowRendering\\ShadowVS.glsl";
static const std::string ShadowRenderFSPath = "G:\\cxcengine\\Src\\GLSL\\ShadowRendering\\ShadowFS.glsl";

static const std::string SceneFBXFile = "G:\\cxcengine\\Projects\\Models\\EN_Building_H_03.FBX";

std::shared_ptr<SubMeshRenderer> CreateShadowRender();
void BindSubMeshRenderer(std::shared_ptr<SubMeshRenderer> pRenderer, const std::vector<std::shared_ptr<CObject>>& Objects);
std::vector<std::shared_ptr<CObject>> CreateObjects(const std::vector<std::shared_ptr<Mesh>>& Meshes);

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

	auto pRendererMgr = RendererManager::GetInstance();
	auto pSceneManager = SceneManager::GetInstance();
	auto pRenderer = CreateShadowRender();
	pRendererMgr->AddRenderer(pRenderer);

	pSceneManager->AddCamera("MainCamera", CameraPos, CameraOrigin, CameraUpVector, ProjectionMatrix);
	pSceneManager->SetCameraActive("MainCamera");

	std::vector<std::shared_ptr<Mesh>> OutMeshes;
	bool bResult = pSceneManager->LoadSceneFromFBX(SceneFBXFile, OutMeshes);
	auto Objects = CreateObjects(OutMeshes);
	if (bResult)
	{
		BindSubMeshRenderer(pRenderer, Objects);
	}

	// Start engine
	GEngine::StartEngine();

	return 0;
}

std::vector<std::shared_ptr<CObject>> CreateObjects(const std::vector<std::shared_ptr<Mesh>>& Meshes)
{
	auto pSceneManager = SceneManager::GetInstance();
	std::vector<std::shared_ptr<CObject>> RetObjects;

	for (auto pMesh : Meshes)
	{
		auto Object = NewObject<CObject>();
		auto StaticMeshComponent = NewObject<CStaticMeshComponent>(pMesh);
		Object->AttachComponent<CStaticMeshComponent>(StaticMeshComponent);
		RetObjects.push_back(Object);
	}

	return RetObjects;
}

void BindSubMeshRenderer(std::shared_ptr<SubMeshRenderer> pRenderer, const std::vector<std::shared_ptr<CObject>>& Objects)
{
	auto pRendererMgr = RendererManager::GetInstance();

	for (auto Object : Objects)
	{
		auto StaticMeshComponent = Object->GetComponent<CStaticMeshComponent>();
		if (StaticMeshComponent)
		{
			size_t SubMeshCount = StaticMeshComponent->GetStaticMesh()->GetSubMeshCount();
			for (size_t Index = 0; Index < SubMeshCount; ++Index)
			{
				auto pSubMesh = StaticMeshComponent->GetStaticMesh()->GetSubMesh(Index);
				pRendererMgr->BindSubMeshRenderer(pSubMesh, pRenderer);
			}
		}
	}
}

std::shared_ptr<SubMeshRenderer> CreateShadowRender()
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

	ShadowMapRender->AddPipeline(ShadowBasePassPipeline);
	ShadowMapRender->AddPipeline(ShadowLightingPassPipeline);

	ShadowMapRender->InitializeRenderer();
	ShadowMapRender->SetShadowMapResolution(512);

	return ShadowMapRender;
}
