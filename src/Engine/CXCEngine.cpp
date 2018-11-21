#include "CXCEngine.h"

namespace cxc
{
	GEngine::GEngine()
	{

	}

	GEngine::~GEngine()
	{

	}

	void GEngine::ConfigureEngineDisplaySettings(const DisplayConfig& DisplayConf)
	{
		auto pWindowMgr = WindowManager::GetInstance();
		if (pWindowMgr)
		{
			pWindowMgr->SetWindowHeight(DisplayConf.WindowHeight);
			pWindowMgr->SetWindowWidth(DisplayConf.WindowWidth);
			pWindowMgr->SetWindowTitle(DisplayConf.ApplicationTitle);
			pWindowMgr->SetWindowPos(DisplayConf.WindowPosX, DisplayConf.WindowPosY);
			pWindowMgr->isDecoraded = DisplayConf.bIsDecorated;
		}
	}

	void GEngine::UseRender(const RenderConfig& RenderConf)
	{
		auto World = World::GetInstance();
		auto pRender = std::make_shared<BaseRender>(RenderConf.VertexShaderPath, RenderConf.FragmentShaderPath);
		if (World)
		{
			World->AddRender(RenderConf.RenderName, pRender);
		}
	}

	void GEngine::InitializeEngine()
	{
		auto World = World::GetInstance();
		if (World)
		{
			// Initialize the engine
			World->Init();
		}
	}

	void GEngine::StartEngine()
	{
		auto World = World::GetInstance();
		if (World)
		{
			// Start to run
			World->run();
		}
	}
}