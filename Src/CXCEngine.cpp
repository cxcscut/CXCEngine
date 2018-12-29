#include "CXCEngine.h"

namespace cxc
{
	GEngine::GEngine()
	{

	}

	GEngine::~GEngine()
	{

	}

	void GEngine::ConfigureEngineDisplaySettings(const DisplayParameters& DisplayConf)
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

	void GEngine::UnBindSubMeshRender(std::shared_ptr<SubMeshRenderer> pSubMeshRender, std::shared_ptr<Mesh> pMesh, uint32_t SubMeshIndex)
	{
		auto pSubMesh = pMesh->GetSubMesh(SubMeshIndex);
		auto pRendererMgr = RendererManager::GetInstance();
		pRendererMgr->UnBindSubMeshRenderer(pSubMesh, pSubMeshRender);
	}

	void GEngine::BindSubMeshRender(std::shared_ptr<SubMeshRenderer> pSubMeshRender, std::shared_ptr<Mesh> pMesh, uint32_t SubMeshIndex)
	{
		auto pSubMesh = pMesh->GetSubMesh(SubMeshIndex);
		auto pRendererMgr = RendererManager::GetInstance();
		pRendererMgr->BindSubMeshRenderer(pSubMesh, pSubMeshRender);
	}

	void GEngine::InitializeEngine()
	{
		auto World = World::GetInstance();
		if (World)
		{
			// Initialize the engine
			World->Initialize();
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