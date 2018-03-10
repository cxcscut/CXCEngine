// FBXSDKTest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#define dDOUBLE
#include "RobotHand.h"
#include "RobotHand.cpp"
#include "Kinematics.cpp"

#define MOVING_STEP 20.0f
#define MOVING_ORIENTATION_STEP (glm::radians(45.0f))

#pragma comment(lib,"CXCENGINE.lib")

using namespace cxc;

static const std::string hand_file = "G:\\cxcengine\\src\\Examples\\RobotSim\\Model\\SZrobotl.obj";
static const std::string plane_file = "G:\\cxcengine\\src\\Examples\\RobotSim\\Model\\plane.obj";
static const std::string table_file = "G:\\cxcengine\\srcsrc\\Examples\\RobotSim\\Model\\table.obj";

static const std::string VertexShaderPath = "G:\\cxcengine\\src\\Engine\\Shader\\StandardVertexShader.glsl";
static const std::string FragmentShaderPath = "G:\\cxcengine\\src\\Engine\\Shader\\Blinn-PhongFS.glsl";
static const std::string ShadowVS = "G:\\cxcengine\\src\\Engine\\Shader\\depthTextureVS.glsl";
static const std::string ShadowFS = "G:\\cxcengine\\src\\Engine\\Shader\\depthTextureFS.glsl";

std::shared_ptr<Robothand> m_LeftPtr, m_RightPtr;
std::shared_ptr<Object3D> Table, Plane;

auto keycallback = [=](int key, int scancode, int action, int mods) {
	auto pEngine = EngineFacade::GetInstance();

	// W pressed
	if (glfwGetKey(pEngine->m_pWindowMgr->GetWindowHandle(), GLFW_KEY_W) == GLFW_PRESS)
	{
		if (m_LeftPtr)
		{
			m_LeftPtr->MovingArmOffset(glm::vec3({MOVING_STEP,0,0}));
		}
	}

	// S pressed
	if (glfwGetKey(pEngine->m_pWindowMgr->GetWindowHandle(), GLFW_KEY_S) == GLFW_PRESS)
	{
		if (m_LeftPtr)
		{
			m_LeftPtr->MovingArmOffset(glm::vec3({ -MOVING_STEP,0,0 }));
		}
	}

	// A pressed
	if (glfwGetKey(pEngine->m_pWindowMgr->GetWindowHandle(), GLFW_KEY_A) == GLFW_PRESS)
	{
		if (m_LeftPtr)
		{
			m_LeftPtr->MovingArmOffset(glm::vec3({0, MOVING_STEP,0 }));
		}
	}

	// D pressed
	if (glfwGetKey(pEngine->m_pWindowMgr->GetWindowHandle(), GLFW_KEY_D) == GLFW_PRESS)
	{
		if (m_LeftPtr)
		{
			m_LeftPtr->MovingArmOffset(glm::vec3({ 0, -MOVING_STEP,0 }));
		}
	}

	// PageUp pressed
	if (glfwGetKey(pEngine->m_pWindowMgr->GetWindowHandle(), GLFW_KEY_PAGE_UP) == GLFW_PRESS)
	{
		if (m_LeftPtr)
		{
			m_LeftPtr->MovingArmOffset(glm::vec3({ 0, 0,MOVING_STEP }));
		}
	}

	// PageDown pressed
	if (glfwGetKey(pEngine->m_pWindowMgr->GetWindowHandle(), GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
	{
		if (m_LeftPtr)
		{
			m_LeftPtr->MovingArmOffset(glm::vec3({ 0, 0,-MOVING_STEP }));
		}
	}

	if (glfwGetKey(pEngine->m_pWindowMgr->GetWindowHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		pEngine->GameOver = true;
	}
};

int main()
{
	glm::vec3 LightPos = glm::vec3(50,50,-120);

	// Accquire engine pointer
	auto pEngine = EngineFacade::GetInstance();
	auto pRender = std::make_shared<BaseRender>(VertexShaderPath, FragmentShaderPath);
	auto pShadow = std::make_shared<ShadowMapRender>(1920,1080, LightPos,ShadowVS,ShadowFS);
	pShadow->SetLightPos(LightPos);
	pShadow->SetLightSourceType(ShadowMapRender::LightSourceType::SpotLight);
	pShadow->SetLightInvDir(LightPos);
	// Engine configuration
	{

		pEngine->SetGravity(0, -0.5f, 0);
		pEngine->m_pWindowMgr->InitGL();

		pEngine->m_pWindowMgr->SetWindowHeight(600);
		pEngine->m_pWindowMgr->SetWindowWidth(800);

		// A "StandardShader must be provided"
		pEngine->addShader("StandardRender", pRender.get());
		pEngine->addShader("ShadowRender",pShadow.get());
		pEngine->m_pWindowMgr->SetWindowTitle("Powered by CXCEngine");
		pEngine->m_pWindowMgr->isDecoraded = true;

		//pEngine->m_pSceneMgr->m_pCamera->eye_pos = glm::vec3(0, 2000, 2000);
		pEngine->m_pSceneMgr->m_pCamera->eye_pos = glm::vec3(0, 80, 80);
		pEngine->m_pSceneMgr->SetLightPos(LightPos);
		//EngineFacade::KeyInputCallBack = keycallback;
		pEngine->SetSceneSize(glm::vec3(0,0,0),4000);

		pEngine->InitWindowPosition(200,200);

		pEngine->MultiThreadingEnable();
	}

	pEngine->Init();

	// Object definition and loading
	{
		/*
		auto LoadRobothand = [&](int type) {
			if (type == ROBOTHAND_LEFT)
				m_LeftPtr = std::make_shared<Robothand>(type, hand_file);
			else
				m_RightPtr = std::make_shared<Robothand>(type, hand_file);
		};

		auto LoadPlane = [&]() {Plane = std::make_shared<Object3D>("plane", plane_file,"env"); };
		auto LoadTable = [&]() {
			Table = std::make_shared<Object3D>("table", table_file, "env");
		};

		std::thread left_hand(LoadRobothand, ROBOTHAND_LEFT);
		std::thread plane(LoadPlane);
		std::thread table(LoadTable);

		left_hand.join();
		plane.join();
		table.join();

		std::cout << m_LeftPtr->GetObjectTreePtr().size() << std::endl;
		
		if (!Plane || !Plane->CheckLoaded()) return 0;
		if (!m_LeftPtr || !m_LeftPtr->CheckLoaded()) return 0;

		pEngine->addObject(Plane, true);
		pEngine->addObject(m_LeftPtr,true);
		pEngine->addObject(Table,true);
		*/

		//auto o1 = std::make_shared<Object3D>("o1","G:\\EngintestObj\\o1.obj");
		//auto o2 = std::make_shared<Object3D>("o2", "G:\\EngintestObj\\o2.obj");
		//auto o3 = std::make_shared<Object3D>("o3", "G:\\EngintestObj\\o3.obj");
		//auto o4 = std::make_shared<Object3D>("o4", "G:\\EngintestObj\\o4.obj");
		//auto o5 = std::make_shared<Object3D>("o5", "G:\\EngintestObj\\o5.obj");

		auto sphere = std::make_shared<Object3D>("o5", "G:\\EngintestObj\\sphere.obj","sphere");
		//auto skeleton = std::make_shared<Object3D>("o5", "G:\\EngintestObj\\skeleton.obj","skeleton");
		auto plane= std::make_shared<Object3D>("plane","G:\\EngintestObj\\plane.obj","env");
		auto wall = std::make_shared<Object3D>("wall","G:\\EngintestObj\\wall.obj","env");
		auto pole = std::make_shared<Object3D>("pole","G:\\EngintestObj\\pole.obj","env");
		sphere->SetObjectGravityMode(1);

		pEngine->addObject(sphere);
		pEngine->addObject(plane,true);
		pEngine->addObject(wall,true);
		pEngine->addObject(pole,true);
	}

	// Adding user code here 
	{
		/*
		Plane->Translation(glm::vec3(0,-1,0));

		m_LeftPtr->InitOriginalDegrees();

		m_LeftPtr->RotateJoint("arm_left2", 90.0f);

		// Set initial pose of robot arm
		m_LeftPtr->SetBaseDegrees({ 0,0,90,0,-90,0 });

		m_LeftPtr->MovingArm(0,-200,500,0,0,0);
		*/
	}

	// Start engine
	pEngine->run();

	// Waiting for stop when condition satisifies
	pEngine->waitForStop();

    return 0;
}
