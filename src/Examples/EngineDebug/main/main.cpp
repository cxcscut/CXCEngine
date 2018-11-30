#include "../../../Engine/CXCEngine.h"
#include "../../../Engine/Rendering/ShadowRender.h"
#include "../../../Engine/Rendering/ShadowRenderPipeline.h"

using namespace cxc;

static const std::string PhongVSFilePath = "G:\\cxcengine\\src\\Engine\\GLSL\\PhongShaderVS.glsl";
static const std::string PhongFSWithNoTextureFilePath = "G:\\cxcengine\\src\\Engine\\GLSL\\NontexturingPhongFS.glsl";
static const std::string PhongFSWithTextureFilePath = "G:\\cxcengine\\src\\Engine\\GLSL\\TexturingPhongFS.glsl";
static const std::string ShadowMapCookingVSFilePath = "G:\\cxcengine\\src\\Engine\\GLSL\\depthTextureVS.glsl.";
static const std::string ShadowMapCookingFSFilePath = "G:\\cxcengine\\src\\Engine\\GLSL\\depthTextureFS.glsl";

static const std::string ShadowVSFilePath = "G:\\cxcengine\\src\\Engine\\GLSL\\ShadowVS.glsl";
static const std::string TexturingShadowOmniLightFSPathFile = "G:\\cxcengine\\src\\Engine\\GLSL\\TexturingShadowOmniLightFS.glsl";
static const std::string NonTexturingShadowOmniLightFSPathFile = "G:\\cxcengine\\src\\Engine\\GLSL\\NonTexturingShadowOmniLightFS.glsl";

static const std::string SceneFBXFile = "G:\\cxcengine\\src\\Examples\\EngineDebug\\main\\EN_Building_H_03.FBX";

std::shared_ptr<MeshRender> UseStandardRender();
std::shared_ptr<MeshRender> UseShadowRender();

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
	auto PhongRender = UseStandardRender();

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
				GEngine::BindMeshRender(PhongRender, pObject.second, i);
			}			
		}
	}

	// Start engine
	GEngine::StartEngine();

	return 0;
}

std::shared_ptr<MeshRender> UseStandardRender()
{
	auto pWorld = World::GetInstance();
	auto pSceneManager = SceneManager::GetInstance();
	auto pRenderMgr = RenderManager::GetInstance();

	// Phong render with no shadows
	auto PhongVS = pRenderMgr->FactoryShader("PhongVS", eShaderType::VERTEX_SHADER, PhongVSFilePath);
	auto PhongFSWithNoTexture = pRenderMgr->FactoryShader("PhongFSWithNoTexture", eShaderType::FRAGMENT_SHADER, PhongFSWithNoTextureFilePath);
	auto PhongFSWithTexture = pRenderMgr->FactoryShader("PhongFSWithTexture", eShaderType::FRAGMENT_SHADER, PhongFSWithTextureFilePath);

	auto TexturingPipeline = NewObject<RenderPipeline>("TexturingPipeline");
	auto NontexturingPipeline = NewObject<RenderPipeline>("NontexturingPipeline");

	TexturingPipeline->AttachShader(PhongVS);
	TexturingPipeline->AttachShader(PhongFSWithTexture);

	NontexturingPipeline->AttachShader(PhongVS);
	NontexturingPipeline->AttachShader(PhongFSWithNoTexture);

	auto PhongRender = NewObject<MeshRender>("PhongRender");
	PhongRender->AddRenderingPipeline(TexturingPipeline);
	PhongRender->AddRenderingPipeline(NontexturingPipeline);
	PhongRender->InitializeRender();
	pRenderMgr->AddRender(PhongRender);

	return PhongRender;
}

std::shared_ptr<MeshRender> UseShadowRender()
{
	auto pWorld = World::GetInstance();
	auto pSceneManager = SceneManager::GetInstance();
	auto pRenderMgr = RenderManager::GetInstance();

	auto ShadowMapRender = NewObject<ShadowRender>("ShadowRender");
	auto ShadowMapPipeline = NewObject<ShadowRenderBasePassPipeline>();
	auto TexturingShadowedMeshPipeline = NewObject<ShadowRenderLightingPassPipeline>("TexturingShadowedMeshPipeline");
	auto NonTexturingShadowedMeshPipeline = NewObject<ShadowRenderLightingPassPipeline>("NonTexturingShadowedMeshPipeline");
	auto ShadowMapCookingVS = pRenderMgr->FactoryShader("ShadowMapCookingVS", eShaderType::VERTEX_SHADER, ShadowMapCookingVSFilePath);
	auto ShadowMapCookingFS = pRenderMgr->FactoryShader("ShadowMapCookingFS", eShaderType::FRAGMENT_SHADER, ShadowMapCookingFSFilePath);
	auto ShadowVS = pRenderMgr->FactoryShader("ShadowVS", eShaderType::VERTEX_SHADER, ShadowVSFilePath);
	auto TexturingShadowOmniLightFS = pRenderMgr->FactoryShader("TexturingShadowOmniLightFS", eShaderType::FRAGMENT_SHADER, TexturingShadowOmniLightFSPathFile);
	auto NonTexturingShadowOmniLightFS = pRenderMgr->FactoryShader("NonTexturingShadowOmniLightFS", eShaderType::FRAGMENT_SHADER, NonTexturingShadowOmniLightFSPathFile);

	ShadowMapPipeline->AttachShader(ShadowMapCookingVS);
	ShadowMapPipeline->AttachShader(ShadowMapCookingFS);
	TexturingShadowedMeshPipeline->AttachShader(ShadowVS);
	TexturingShadowedMeshPipeline->AttachShader(TexturingShadowOmniLightFS);
	NonTexturingShadowedMeshPipeline->AttachShader(ShadowVS);
	NonTexturingShadowedMeshPipeline->AttachShader(NonTexturingShadowOmniLightFS);

	ShadowMapRender->AddRenderingPipeline(ShadowMapPipeline);
	ShadowMapRender->AddRenderingPipeline(TexturingShadowedMeshPipeline);
	ShadowMapRender->AddRenderingPipeline(NonTexturingShadowedMeshPipeline);

	ShadowMapRender->InitializeRender();
	ShadowMapRender->SetShadowMapResolution(512);
	pRenderMgr->AddRender(ShadowMapRender);

	return ShadowMapRender;
}

