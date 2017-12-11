// FBXSDKTest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "EngineFacade\EngineFacade.h"

#pragma comment(lib,"CXCENGINE.lib")

using namespace cxc;

static const std::string plane_file = "Model\\plane.obj";
static const std::string sphere_file = "Model\\sphere.obj";
static const std::string VertexShaderPath = "C:\\Users\\39317\\Desktop\\git\\cxcengine\\src\\Engine\\Shader\\StandardVertexShader.glsl";
static const std::string FragmentShaderPath = "C:\\Users\\39317\\Desktop\\git\\cxcengine\\src\\Engine\\Shader\\StandardFragmentShader.glsl";

int main()
{
	auto pEngine = EngineFacade::GetInstance();

	auto plane = std::make_shared<Object3D>("plane",plane_file);
	auto sphere = std::make_shared<Object3D>("sphere",sphere_file);
	if (!plane || !plane->CheckLoaded()) return 0;
	if (!sphere || !sphere->CheckLoaded()) return 0;

	pEngine->m_pSceneMgr->AddObject("plane", plane);
	pEngine->m_pSceneMgr->AddObject("sphere", sphere);

	plane->SetObjectGravityMode(0);
	sphere->SetObjectGravityMode(1);

	pEngine->m_pWindowMgr->InitGL();

	pEngine->m_pWindowMgr->SetWindowHeight(600);
	pEngine->m_pWindowMgr->SetWindowWidth(800);

	pEngine->SetVertexShaderPath(VertexShaderPath);
	pEngine->SetFragmentShaderPath(FragmentShaderPath);

	pEngine->m_pWindowMgr->SetWindowTitle("Engine test");
	pEngine->m_pWindowMgr->isDecoraded = true;

	pEngine->m_pSceneMgr->m_pCamera->eye_pos = glm::vec3(20, 20, 20);
	pEngine->m_pSceneMgr->m_pCamera->ComputeViewMatrix();

	pEngine->run(false);

    return 0;
}

