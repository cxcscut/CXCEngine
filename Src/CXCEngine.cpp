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

	void GEngine::SetLogicEntry(std::function<void(void)> LogicEntry)
	{
		auto pWorld = World::GetInstance();
		auto pLogicWorld = pWorld->GetLogicWorld();
		assert(pLogicWorld);
		if (pLogicWorld)
		{
			pLogicWorld->SetLogicEntry(LogicEntry);
		}
	}

	void GEngine::BindMeshRender(std::shared_ptr<MeshRender> pRender, std::shared_ptr<Object3D> pObject, uint32_t MeshIndex)
	{
		auto pMesh = pObject->GetMesh(MeshIndex);
		if (pMesh)
		{
			pMesh->BindMeshRender(pRender);
		}
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