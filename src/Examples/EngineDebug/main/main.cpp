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

	DisplayConfig DisplayConf;
	DisplayConf.ApplicationTitle = "Powered by CXCEngine";
	DisplayConf.bIsDecorated = true;
	DisplayConf.WindowHeight = 600;
	DisplayConf.WindowWidth = 800;
	DisplayConf.WindowPosX = 200; 
	DisplayConf.WindowPosY = 200;
	GEngine::ConfigureEngineDisplaySettings(DisplayConf);

	RenderConfig RenderConf;
	RenderConf.RenderName = "StandardRender";
	RenderConf.VertexShaderPath = VertexShaderPath;
	RenderConf.FragmentShaderPath = FragmentShaderPath;
	GEngine::UseRender(RenderConf);

	auto pSceneManager = SceneManager::GetInstance();

	pSceneManager->m_pCamera->EyePosition = glm::vec3(0, 250, 30);
	pSceneManager->SetLightPos(LightPos);

	GEngine::InitializeEngine();

	GLboolean lResult = pSceneManager->LoadSceneFromFBX(FBXFile);

	// Start engine
	GEngine::StartEngine();

	return 0;
}


