#include "../../../Engine/CXCEngine.h"
#include "../../../Engine/Rendering/ForwardRender.h"
#include "../../../Engine/Rendering/ShadowRender.h"
#include "../../../Engine/Rendering/ShadowRenderPipeline.h"

using namespace cxc;

static const std::string ForwardRenderVSPath = "G:\\cxcengine\\src\\Engine\\GLSL\\ForwardRendering\\ForwardRenderVS.glsl";
static const std::string ForwardRenderFSPath = "G:\\cxcengine\\src\\Engine\\GLSL\\ForwardRendering\\ForwardRenderFS.glsl";

static const std::string ShadowRenderDepthVSPath = "G:\\cxcengine\\src\\Engine\\GLSL\\ShadowRendering\\depthTextureVS.glsl";
static const std::string ShadowRenderDepthFSPath = "G:\\cxcengine\\src\\Engine\\GLSL\\ShadowRendering\\depthTextureFS.glsl";
static const std::string ShadowRenderVSPath = "G:\\cxcengine\\src\\Engine\\GLSL\\ShadowRendering\\ShadowVS.glsl";
static const std::string ShadowRenderFSPath = "G:\\cxcengine\\src\\Engine\\GLSL\\ShadowRendering\\ShadowFS.glsl";

static const std::string SceneFBXFile = "G:\\cxcengine\\src\\Examples\\EngineDebug\\main\\EN_Building_H_03.FBX";

std::shared_ptr<MeshRender> CreateStandardRender();
std::shared_ptr<MeshRender> CreateShadowRender();

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

	GEngine::SetCamera(CameraPos , CameraOrigin, CameraUpVector, ProjectionMatrix);
	auto pSceneManager = SceneManager::GetInstance();

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
				GEngine::BindMeshRender(CreateShadowRender() , pObject.second, i);
			}			
		}
	}

	// Start engine
	GEngine::StartEngine();

	return 0;
}

std::shared_ptr<MeshRender> CreateStandardRender()
{
	auto pWorld = World::GetInstance();
	auto pSceneManager = SceneManager::GetInstance();
	auto pRenderMgr = RenderManager::GetInstance();

	// Foward Phong render with no shadows
	auto ForwardPhongVS = pRenderMgr->FactoryShader("ForwardVS", eShaderType::VERTEX_SHADER, ForwardRenderVSPath);
	auto ForwardPhongFS = pRenderMgr->FactoryShader("ForwardFS", eShaderType::FRAGMENT_SHADER, ForwardRenderFSPath);

	auto ForwardPhongPipeline = NewObject<ForwardRenderPipeline>("ForwardPhongPipeline");
	ForwardPhongPipeline->AttachShader(ForwardPhongVS);
	ForwardPhongPipeline->AttachShader(ForwardPhongFS);

	auto PhongRender = NewObject<ForwardRender>("ForwardPhongRender");
	PhongRender->SetForwardRenderPipeline(ForwardPhongPipeline);
	PhongRender->InitializeRender();
	pRenderMgr->AddRender(PhongRender);

	return PhongRender;
}

std::shared_ptr<MeshRender> CreateShadowRender()
{
	auto pWorld = World::GetInstance();
	auto pSceneManager = SceneManager::GetInstance();
	auto pRenderMgr = RenderManager::GetInstance();

	auto ShadowMapRender = NewObject<ShadowRender>("ShadowRender");
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
	pRenderMgr->AddRender(ShadowMapRender);

	return ShadowMapRender;
}

