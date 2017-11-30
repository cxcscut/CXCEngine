// FBXSDKTest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "EngineFacade\EngineFacade.h"

#pragma comment(lib,"CXCENGINE.lib")

using namespace cxc;

static const std::string model_path = "Model\\skeleton.obj";
static const std::string VertexShaderPath = "C:\\Users\\39317\\Desktop\\git\\cxcengine\\src\\Engine\\Shader\\StandardVertexShader.glsl";
static const std::string FragmentShaderPath = "C:\\Users\\39317\\Desktop\\git\\cxcengine\\src\\Engine\\Shader\\StandardFragmentShader.glsl";

int main()
{
	auto pEngine = EngineFacade::GetInstance();

	auto pTable = std::make_shared<Object3D>("skeleton",model_path);
	if (!pTable || !pTable->CheckLoaded()) return 0;

	pEngine->m_pSceneMgr->AddObject("skeleton",pTable);

	pEngine->m_pWindowMgr->InitGL();

	pEngine->m_pWindowMgr->SetWindowHeight(600);
	pEngine->m_pWindowMgr->SetWindowWidth(800);

	pEngine->SetVertexShaderPath(VertexShaderPath);
	pEngine->SetFragmentShaderPath(FragmentShaderPath);

	pEngine->m_pWindowMgr->SetWindowTitle("Animation test");
	pEngine->m_pWindowMgr->isDecoraded = true;
	
	pEngine->run(false);

    return 0;
}

