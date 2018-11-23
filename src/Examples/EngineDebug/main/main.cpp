#include "../../../Engine/CXCEngine.h"

using namespace cxc;

static const std::string VertexShaderPath = "G:\\cxcengine\\src\\Engine\\GLSL\\StandardVertexShader.glsl";
static const std::string FragmentShaderPath = "G:\\cxcengine\\src\\Engine\\GLSL\\Blinn-PhongFS.glsl";
static const std::string ShadowVS = "G:\\cxcengine\\src\\Engine\\GLSL\\depthTextureVS.glsl";
static const std::string ShadowFS = "G:\\cxcengine\\src\\Engine\\GLSL\\depthTextureFS.glsl";
static const std::string FBXFile = "G:\\cxcengine\\src\\Examples\\EngineDebug\\main\\test.FBX";

int main()
{
	glm::vec3 LightPos = glm::vec3(80, 80, 0);

	DisplayParameters DisplayConf;
	DisplayConf.ApplicationTitle = "Powered by CXCEngine";
	DisplayConf.bIsDecorated = true;
	DisplayConf.WindowHeight = 600;
	DisplayConf.WindowWidth = 800;
	DisplayConf.WindowPosX = 200; 
	DisplayConf.WindowPosY = 200;
	GEngine::ConfigureEngineDisplaySettings(DisplayConf);
	GEngine::InitializeEngine();

	auto pShaderMgr = ShaderManager::GetInstance();
	auto pRenderMgr = RenderManager::GetInstance();
	auto VertexShader = pShaderMgr->FactoryShader("PhongVertexShader", eShaderType::VERTEX_SHADER, VertexShaderPath);
	auto FragmentShader = pShaderMgr->FactoryShader("PhongFragmentShader", eShaderType::FRAGMENT_SHADER, FragmentShaderPath);

	auto SceneRenderingPipeline = NewObject<RenderPipeline>("PhongRenderPipeline");
	SceneRenderingPipeline->AttachShader(VertexShader);
	SceneRenderingPipeline->AttachShader(FragmentShader);
	auto PhongRender = NewObject<MeshRender>("PhongRender");

	PhongRender->AddRenderingPipeline(SceneRenderingPipeline);
	PhongRender->AddLight(LightPos, -LightPos, eLightType::OmniDirectional, eInteractionType::Static);
	bool bRenderInitialize = PhongRender->InitializeRender();
	pRenderMgr->AddRender(PhongRender);
	GEngine::SetActiveRender(PhongRender);

	auto pSceneManager = SceneManager::GetInstance();
	pSceneManager->pCamera->EyePosition = glm::vec3(0, 250, 30);

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


