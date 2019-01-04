#include "Utilities/DebugDrawHelper.h"
#include "Utilities/GeometryUtil.h"
#include "Geometry/DebugMesh.h"
#include "Scene/SceneManager.h"

namespace cxc
{
	DebugDrawHelper::DebugDrawHelper()
	{

	}

	DebugDrawHelper::~DebugDrawHelper()
	{

	}

	void DebugDrawHelper::AddDebugSphereShape(const glm::vec3& Pos, float Radius, uint32_t Segment, const glm::vec3& Color, float Persistence)
	{
		auto pSceneMgr = SceneManager::GetInstance();

		// Create sphere mesh
		auto DebugSphereMesh = GeometryUtil::MakeSphere(Radius, Pos, Segment, Color);
		DebugSphereMesh->SetPersistance(Persistence);

		pSceneMgr->pRendererMgr->BindDebugMesh(DebugSphereMesh);

		pSceneMgr->AddDebugMesh(DebugSphereMesh);
	}

	void DebugDrawHelper::AddDebugCubeShape(const glm::vec3 &Pos, const glm::vec3 Extents, const glm::vec3& Color, float Persistence)
	{
		auto pSceneMgr = SceneManager::GetInstance();

		// Create cube mesh
		auto DebugCubeMesh = GeometryUtil::MakeBox(Pos, Extents, Color);
		DebugCubeMesh->SetPersistance(Persistence);

		pSceneMgr->pRendererMgr->BindDebugMesh(DebugCubeMesh);

		pSceneMgr->AddDebugMesh(DebugCubeMesh);
	}

	void DebugDrawHelper::RemoveDebugShapes()
	{
		auto pSceneMgr = SceneManager::GetInstance();
		pSceneMgr->FlushDebugMeshes();
	}
}