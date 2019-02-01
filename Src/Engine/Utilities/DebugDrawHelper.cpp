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

	void DebugDrawHelper::AddDebugSphereShape(const glm::vec3& Pos, float Radius, uint32_t Segment, const glm::vec3& SphereColor, float Persistence)
	{
		auto pSceneMgr = SceneManager::GetInstance();

		std::vector<glm::vec3> SphereVertices;
		std::vector<uint32_t> SphereIndices;

		// Create sphere mesh
		GeometryUtil::MakeSphere(Radius, Segment, SphereVertices, SphereIndices);

		auto DebugSphereMesh = NewObject<DebugMesh>(SphereVertices);
		DebugSphereMesh->SetDebugMeshColor(SphereColor);
		DebugSphereMesh->Translate(Pos);
		auto pSubMesh = NewObject<SubMesh>(SphereIndices);
		pSubMesh->SetShadingMode(eShadingMode::WireframeMode);
		DebugSphereMesh->AddSubMesh(pSubMesh);

		DebugSphereMesh->SetPersistance(Persistence);

		pSceneMgr->pRendererMgr->BindDebugMesh(DebugSphereMesh);
		pSceneMgr->AddDebugMesh(DebugSphereMesh);
	}

	void DebugDrawHelper::AddDebugCubeShape(const glm::vec3 &Pos, const glm::vec3 Extents, const glm::vec3& CubeColor, float Persistence)
	{
		auto pSceneMgr = SceneManager::GetInstance();

		std::vector<glm::vec3> CubeVertices;
		std::vector<uint32_t> CubeIndices;

		// Create cube mesh
		GeometryUtil::MakeBox(Extents, CubeVertices, CubeIndices);
		std::shared_ptr<DebugMesh> DebugCubeMesh = NewObject<DebugMesh>(CubeVertices);
		DebugCubeMesh->SetDebugMeshColor(CubeColor);
		DebugCubeMesh->Translate(Pos);

		auto pSubMesh = NewObject<SubMesh>(CubeIndices);
		pSubMesh->SetShadingMode(eShadingMode::WireframeMode);
		DebugCubeMesh->AddSubMesh(pSubMesh);
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