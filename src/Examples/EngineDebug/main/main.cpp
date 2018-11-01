#include "../../../Engine/EngineFacade/EngineFacade.h"

#define dDOUBLE
#define MOVING_STEP 20.0f
#define MOVING_ORIENTATION_STEP (glm::radians(45.0f))

using namespace cxc;

static const std::string VertexShaderPath = "G:\\cxcengine\\src\\Engine\\Shader\\StandardVertexShader.glsl";
static const std::string FragmentShaderPath = "G:\\cxcengine\\src\\Engine\\Shader\\Blinn-PhongFS.glsl";
static const std::string ShadowVS = "G:\\cxcengine\\src\\Engine\\Shader\\depthTextureVS.glsl";
static const std::string ShadowFS = "G:\\cxcengine\\src\\Engine\\Shader\\depthTextureFS.glsl";

int main()
{
	glm::vec3 LightPos = glm::vec3(80, 80, 0);

	// Accquire engine pointer
	auto pEngine = EngineFacade::GetInstance();
	auto pRender = std::make_shared<BaseRender>(VertexShaderPath, FragmentShaderPath);
	auto pShadow = std::make_shared<ShadowMapRender>(1920, 1920, ShadowVS, ShadowFS);
	pShadow->CreateLightInfo(LightPos, -LightPos, eLightType::OmniDirectional, eInteractionType::Static);
	// Engine configuration
	{

		pEngine->SetGravity(0, -0.5f, 0);
		pEngine->m_pWindowMgr->InitGL();

		pEngine->m_pWindowMgr->SetWindowHeight(600);
		pEngine->m_pWindowMgr->SetWindowWidth(800);

		// A "StandardShader must be provided"
		pEngine->addShader("StandardRender", pRender.get());
		pEngine->addShader("ShadowRender", pShadow.get());
		pEngine->m_pWindowMgr->SetWindowTitle("Powered by CXCEngine");
		pEngine->m_pWindowMgr->isDecoraded = true;

		//pEngine->m_pSceneMgr->m_pCamera->eye_pos = glm::vec3(0, 2000, 2000);
		pEngine->m_pSceneMgr->m_pCamera->eye_pos = glm::vec3(0, 80, 80);
		pEngine->m_pSceneMgr->SetLightPos(LightPos);
		pEngine->SetSceneSize(glm::vec3(0, 0, 0), 4000);

		pEngine->InitWindowPosition(200, 200);

		pEngine->MultiThreadingEnable();
	}

	pEngine->Init();

	auto sphere = std::make_shared<Object3D>("o5", "G:\\cxcengine\\src\\Examples\\FBXSDK-Test\\Model\\sphere.obj", "sphere");
	auto plane = std::make_shared<Object3D>("plane", "G:\\cxcengine\\src\\Examples\\FBXSDK-Test\\Model\\plane.obj", "env");
	auto wall = std::make_shared<Object3D>("wall", "G:\\cxcengine\\src\\Examples\\FBXSDK-Test\\Model\\wall.obj", "env");
	auto pole = std::make_shared<Object3D>("pole", "G:\\cxcengine\\src\\Examples\\FBXSDK-Test\\Model\\pole.obj", "env");

	sphere->LoadingObjectModel();
	plane->LoadingObjectModel();
	wall->LoadingObjectModel();
	pole->LoadingObjectModel();

	//sphere->SetObjectGravityMode(1);

	pEngine->addObject(sphere);
	pEngine->addObject(plane, true);
	pEngine->addObject(wall, true);
	pEngine->addObject(pole, true);

	// Start engine
	pEngine->run();

	// Waiting for stop when condition satisifies
	pEngine->waitForStop();

	return 0;
}


