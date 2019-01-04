#include "Utilities/DebugDrawHelper.h"
#include "Utilities/GeometryUtil.h"
#include "Scene/SceneManager.h"

namespace cxc
{
	DebugDrawHelper::DebugDrawHelper()
	{

	}

	DebugDrawHelper::~DebugDrawHelper()
	{

	}

	void DebugDrawHelper::AddDebugSphereMesh(const glm::vec3& Pos, float Radius, uint32_t Segment, const glm::vec3& Color)
	{
		auto pSceneMgr = SceneManager::GetInstance();

		// Create sphere mesh
		auto DebugSphereMesh = GeometryUtil::MakeSphere(Radius, Pos, Segment, Color);
		pSceneMgr->pRendererMgr->BindDebugMesh(DebugSphereMesh);

		pSceneMgr->AddMesh(DebugSphereMesh);
	}

	void DebugDrawHelper::AddDebugCubeMesh(const glm::vec3 &Pos, const glm::vec3 Extents, const glm::vec3& Color)
	{
		auto pSceneMgr = SceneManager::GetInstance();

		// Create cube mesh
		auto DebugCubeMesh = GeometryUtil::MakeBox(Pos, Extents, Color);
		pSceneMgr->pRendererMgr->BindDebugMesh(DebugCubeMesh);

		pSceneMgr->AddMesh(DebugCubeMesh);
	}

	void DebugDrawHelper::RemoveDebugMeshes()
	{
		auto pSceneMgr = SceneManager::GetInstance();
		pSceneMgr->FlushDebugMeshes();
	}
}