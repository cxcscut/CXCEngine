#include "../../../Engine/EngineFacade/EngineFacade.h"

#define dDOUBLE
#define MOVING_STEP 20.0f
#define MOVING_ORIENTATION_STEP (glm::radians(45.0f))

using namespace cxc;

static const std::string VertexShaderPath = "G:\\cxcengine\\src\\Engine\\Shader\\StandardVertexShader.glsl";
static const std::string FragmentShaderPath = "G:\\cxcengine\\src\\Engine\\Shader\\Blinn-PhongFS.glsl";
static const std::string ShadowVS = "G:\\cxcengine\\src\\Engine\\Shader\\depthTextureVS.glsl";
static const std::string ShadowFS = "G:\\cxcengine\\src\\Engine\\Shader\\depthTextureFS.glsl";
static const std::string FBXFile = "G:\\cxcengine\\src\\Examples\\EngineDebug\\main\\test.FBX";

int main()
{
	glm::vec3 LightPos = glm::vec3(80, 80, 0);

	// Accquire engine pointer
	auto pEngine = EngineFacade::GetInstance();
	auto pSceneManager = SceneManager::GetInstance();
	auto pRender = std::make_shared<BaseRender>(VertexShaderPath, FragmentShaderPath);
	auto pShadow = std::make_shared<ShadowMapRender>(1920, 1920, ShadowVS, ShadowFS);
	pShadow->CreateLightInfo(LightPos, -LightPos, eLightType::OmniDirectional, eInteractionType::Static);
	
	//pEngine->SetGravity(0, -0.5f, 0);
	pEngine->m_pWindowMgr->InitGL();

	pEngine->m_pWindowMgr->SetWindowHeight(600);
	pEngine->m_pWindowMgr->SetWindowWidth(800);

	pEngine->addShader("StandardRender", pRender.get());
	//pEngine->addShader("ShadowRender", pShadow.get());
	pEngine->m_pWindowMgr->SetWindowTitle("Powered by CXCEngine");
	pEngine->m_pWindowMgr->isDecoraded = true;

	pEngine->m_pSceneMgr->m_pCamera->EyePosition = glm::vec3(0, 250, 30);
	pEngine->m_pSceneMgr->SetLightPos(LightPos);

	pEngine->InitWindowPosition(200, 200);

	pEngine->MultiThreadingEnable();
	
	pEngine->Init();

	GLboolean lResult = pSceneManager->LoadSceneFromFBX(FBXFile);

	auto pSphere = pSceneManager->GetObject3D("Cylinder001");
	pSphere->RotateWithArbitraryAxis(pSphere->GetPivot(), glm::vec3(0,1,0), 45.0f);

	// Start engine
	pEngine->run();

	// Waiting for stop when condition satisifies
	pEngine->waitForStop();

	return 0;
}


