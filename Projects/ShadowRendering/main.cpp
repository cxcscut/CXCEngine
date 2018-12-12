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

std::shared_ptr<MeshRenderer> CreateShadowRender();

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

	auto pRenderMgr = RendererManager::GetInstance();
	auto pSceneManager = SceneManager::GetInstance();
	auto pRender = CreateShadowRender();
	pRenderMgr->AddRender(pRender);

	pSceneManager->AddCamera("MainCamera", CameraPos, CameraOrigin, CameraUpVector, ProjectionMatrix);
	pSceneManager->SetCameraActive("MainCamera");

	bool bResult = pSceneManager->LoadSceneFromFBX(SceneFBXFile);
	if (bResult)
	{
		// Bind mesh render
		auto ObjectMap = pSceneManager->GetObjectMap();
		for (auto pObject : ObjectMap)
		{
			auto MeshCount = pObject.second->GetMeshCount();
			for (size_t i = 0; i < MeshCount; ++i)
			{
				GEngine::BindMeshRender(pRender, pObject.second, i);
			}
		}
	}

	// Start engine
	GEngine::StartEngine();

	return 0;
}

std::shared_ptr<MeshRenderer> CreateShadowRender()
{
	auto pRenderMgr = RendererManager::GetInstance();

	auto ShadowMapRender = NewObject<ShadowRenderer>("ShadowRenderer");
	auto ShadowBasePassPipeline = NewObject<ShadowRenderBasePassPipeline>();
	auto ShadowLightingPassPipeline = NewObject<ShadowRenderLightingPassPipeline>();

	auto ShadowMapCookingVS = pRenderMgr->FactoryShader("ShadowMapCookingVS", eShaderType::VERTEX_SHADER, ShadowRenderDepthVSPath);
	auto ShadowMapCookingFS = pRenderMgr->FactoryShader("ShadowMapCookingFS", eShaderType::FRAGMENT_SHADER, ShadowRenderDepthFSPath);
	auto ShadowVS = pRenderMgr->FactoryShader("ShadowVS", eShaderType::VERTEX_SHADER, ShadowRenderVSPath);
	auto ShadowFS = pRenderMgr->FactoryShader("ShadowFS", eShaderType::FRAGMENT_SHADER, ShadowRenderFSPath);

	ShadowBasePassPipeline->AttachShader(ShadowMapCookingVS);
	ShadowBasePassPipeline->AttachShader(ShadowMapCookingFS);
	ShadowLightingPassPipeline->AttachShader(ShadowVS);
	ShadowLightingPassPipeline->AttachShader(ShadowFS);

	ShadowMapRender->SetBasePassPipeline(ShadowBasePassPipeline);
	ShadowMapRender->SetLightingPassPipeline(ShadowLightingPassPipeline);

	ShadowMapRender->InitializeRender();
	ShadowMapRender->SetShadowMapResolution(512);

	return ShadowMapRender;
}
