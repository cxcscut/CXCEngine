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

			pWindowMgr->x_pos = DisplayConf.WindowPosX; pWindowMgr->y_pos = DisplayConf.WindowPosY;
		}
	}

	void GEngine::SetActiveRender(std::shared_ptr<Render> pRender)
	{
		auto pRenderMgr = SceneManager::GetInstance()->pRenderMgr;
		pRenderMgr->SetCurrentUsedRender(pRender);
	}

	void GEngine::AddRender(std::shared_ptr<Render> pRender)
	{
		auto World = World::GetInstance();
		if (World)
		{
			World->AddRender(pRender->GetRenderName(), pRender);
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