#include "../../../Engine/CXCEngine.h"

using namespace cxc;

static const std::string PhongVSFilePath = "G:\\cxcengine\\src\\Engine\\GLSL\\PhongShaderVS.glsl";
static const std::string PhongFSWithNoTextureFilePath = "G:\\cxcengine\\src\\Engine\\GLSL\\NontexturingPhongFS.glsl";
static const std::string PhongFSWithTextureFilePath = "G:\\cxcengine\\src\\Engine\\GLSL\\TexturingPhongFS.glsl";
static const std::string ShadowVSFilePath = "G:\\cxcengine\\src\\Engine\\GLSL\\depthTextureVS.glsl.";
static const std::string ShadowFSFilePath = "G:\\cxcengine\\src\\Engine\\GLSL\\depthTextureFS.glsl";
static const std::string FBXFile = "G:\\cxcengine\\src\\Examples\\EngineDebug\\main\\material.FBX";

int main()
{
	glm::vec3 LightPos = glm::vec3(80, 80, 0);
	glm::vec3 CameraPos = glm::vec3(0, 250, 30);
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

	auto pSceneManager = SceneManager::GetInstance();
	pSceneManager->AddLight("MainLight", LightPos, -LightPos, eLightType::OmniDirectional);

	auto pRenderMgr = RenderManager::GetInstance();
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
	bool bRenderInitialize = PhongRender->InitializeRender();

	pRenderMgr->AddRender(PhongRender);
	GEngine::SetActiveRender(PhongRender);

	GEngine::SetCamera(CameraPos , CameraOrigin, CameraUpVector, ProjectionMatrix);

	bool bResult = pSceneManager->LoadSceneFromFBX(FBXFile);
	if (bResult)
	{
		// Bind mesh render
		auto ObjectMap = pSceneManager->GetObjectMap();
		for (auto pObject : ObjectMap)
		{
			if (pObject.second)
			{
				auto MeshCount = pObject.second->GetMeshCount();
				for (size_t i = 0; i < MeshCount; ++i)
				{
					GEngine::BindMeshRender(PhongRender, pObject.second, i);
				}
			}
		}
	}

	// Start engine
	GEngine::StartEngine();

	return 0;
}


