#include "../../../Engine/CXCEngine.h"

using namespace cxc;

static const std::string VertexShaderPath = "G:\\cxcengine\\src\\Engine\\Shader\\StandardVertexShader.glsl";
static const std::string FragmentShaderPath = "G:\\cxcengine\\src\\Engine\\Shader\\Blinn-PhongFS.glsl";
static const std::string ShadowVS = "G:\\cxcengine\\src\\Engine\\Shader\\depthTextureVS.glsl";
static const std::string ShadowFS = "G:\\cxcengine\\src\\Engine\\Shader\\depthTextureFS.glsl";
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

	auto PhongRender = NewObject<MeshRender>("PhongRender");
	auto SceneRenderingPipeline = NewObject<RenderPipeline>();
	SceneRenderingPipeline->SetVertexShaderPath(VertexShaderPath);
	SceneRenderingPipeline->SetFragmentShaderPath(FragmentShaderPath);
	PhongRender->AddRenderingPipeline(SceneRenderingPipeline);
	PhongRender->AddLight(LightPos, -LightPos, eLightType::OmniDirectional, eInteractionType::Static);
	GEngine::AddRender(PhongRender);
	GEngine::SetActiveRender(PhongRender);

	auto pSceneManager = SceneManager::GetInstance();
	pSceneManager->pCamera->EyePosition = glm::vec3(0, 250, 30);

	GEngine::InitializeEngine();

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


