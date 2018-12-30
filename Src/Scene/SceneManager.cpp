#include "Scene/SceneManager.h"
#include "Geometry/Mesh.h"
#include "World/World.h"
#include <iostream>

namespace cxc {

	SceneManager::SceneManager()
		: MeshMap(),
		m_Boundary()
	{
		pMaterialMgr = MaterialManager::GetInstance();
		pRendererMgr = RendererManager::GetInstance();
	}

	SceneManager::~SceneManager()
	{
		MeshMap.clear();
	}

	void SceneManager::AddCamera(const std::string& CameraName, 
		const glm::vec3 &eye, const glm::vec3 &origin, const glm::vec3 &up,
		const glm::mat4 &ProjectionMatrix) noexcept
	{
		auto pNewCamera = std::make_shared<Camera>();
		pNewCamera->CameraName = CameraName;
		pNewCamera->EyePosition = eye;
		pNewCamera->CameraOrigin = origin;
		pNewCamera->UpVector = up;
		pNewCamera->SetAllMatrix(glm::lookAt(eye, origin, up), ProjectionMatrix);
		pNewCamera->ComputeAngles();
		pNewCamera->ComputeViewMatrix();
		AddCamera(pNewCamera);
	}

	void SceneManager::UpdateBoundary(const CXCRect3D &AABB) noexcept
	{
		m_Boundary.max.x = std::fmax(m_Boundary.max.x, AABB.max.x);
		m_Boundary.max.y = std::fmax(m_Boundary.max.y, AABB.max.y);
		m_Boundary.max.z = std::fmax(m_Boundary.max.z, AABB.max.z);

		m_Boundary.min.x = std::fmin(m_Boundary.min.x, AABB.min.x);
		m_Boundary.min.y = std::fmin(m_Boundary.min.y, AABB.min.y);
		m_Boundary.min.z = std::fmin(m_Boundary.min.z, AABB.min.z);
	}

	void SceneManager::RemoveMesh(const std::shared_ptr<Mesh> pMesh)
	{
		auto Iter = MeshMap.find(pMesh->GetMeshName());
		if (Iter != MeshMap.end())
		{
			MeshMap.erase(Iter);
		}
	}

	std::shared_ptr<Camera> SceneManager::GetCamera(size_t Index)
	{
		if (Index < Cameras.size())
			return Cameras[Index];
		else
			return nullptr;
	}

	void SceneManager::AddCamera(std::shared_ptr<Camera> Camera)
	{
		if (Camera)
		{
			Cameras.push_back(Camera);
		}
	}

	void SceneManager::AddMesh(const std::vector<std::shared_ptr<Mesh>> Meshes)
	{
		for (auto Mesh : Meshes)
		{
			if (Mesh)
			{
				MeshMap.insert(std::make_pair(Mesh->GetMeshName(), Mesh->shared_from_this()));
			}
		}
	}

	void SceneManager::AddMesh(const std::shared_ptr<Mesh> pMesh) noexcept
	{
		if(pMesh != nullptr)
			MeshMap.insert(std::make_pair(pMesh->GetMeshName(), pMesh));
	}

	void SceneManager::RenderScene() noexcept
	{
		// Clear the renderer context for the current rendering
		pRendererMgr->ClearRendererContext();

		// Allocates buffers for rendering
		for (auto pMesh : MeshMap)
		{
			pMesh.second->AllocateBuffers();

			// Add sub-meshes to the render context
			size_t SubMeshCount = pMesh.second->GetSubMeshCount();
			for (size_t Index = 0; Index < SubMeshCount; ++Index)
			{
				auto pSubMesh = pMesh.second->GetSubMesh(Index);
				pRendererMgr->AddSubMeshToRendererContext(pSubMesh);
			}
		}

		// Rendering scene
		for (auto pRendererIter : pRendererMgr->RenderersMap)
		{
			auto RendererContextIter = pRendererMgr->RendererContextsMap.find(pRendererIter.second->GetRendererName());
			if (RendererContextIter != pRendererMgr->RendererContextsMap.end())
			{
				pRendererMgr->UseRenderer(pRendererIter.second);
				pRendererIter.second->Render(RendererContextIter->second);
			}
		}

		// Release buffers
		for (auto pMesh : MeshMap)
		{
			pMesh.second->ReleaseBuffers();
		}
	}

	void SceneManager::RemoveCamera(std::shared_ptr<Camera> pCamera)
	{
		for (auto Iter = Cameras.end() - 1; Iter >= Cameras.begin(); --Iter)
		{
			if ((*Iter) == pCamera)
			{
				Cameras.erase(Iter);
			}
		}

		Cameras.shrink_to_fit();
	}

	void SceneManager::RemoveLight(std::shared_ptr<LightSource> pLight)
	{
		for (auto iter = Lights.end() - 1; iter >= Lights.begin(); --iter)
		{
			if ((*iter) == pLight)
			{
				Lights.erase(iter);
			}
		}

		Lights.shrink_to_fit();
	}

	std::shared_ptr<LightSource> SceneManager::GetLight(uint32_t LightIndex)
	{
		if (GetLightCount() <= LightIndex)
		{
			return nullptr;
		}
		else
			return Lights[LightIndex];
	}

	std::shared_ptr<LightSource> SceneManager::GetLight(const std::string& LightName)
	{
		for (auto pLight : Lights)
		{
			if (pLight && pLight->GetLightName() == LightName)
			{
				return pLight;
			}
		}
		
		return nullptr;
	}

	void SceneManager::AddLight(std::shared_ptr<LightSource> pNewLight)
	{
		if (!pNewLight)
			return;

		bool bIsLightExist = false;
		for (auto pLight : Lights)
		{
			if (pLight->GetLightName() == pNewLight->GetLightName())
			{
				bIsLightExist = true;
				break;
			}
		}

		if (!bIsLightExist)
		{
			Lights.push_back(pNewLight);
		}
	}

	std::shared_ptr<Camera> SceneManager::GetCurrentActiveCamera()
	{
		return CurrentActiveCamera;
	}

	void SceneManager::SetCameraActive(std::shared_ptr<Camera> pCamera)
	{
		CurrentActiveCamera = pCamera;
	}

	void SceneManager::AddLight(const std::string& Name, const glm::vec3& LightPosition, const glm::vec3& TargetPos, float LightIntensity, eLightType Type)
	{
		auto pNewLight = NewObject<LightSource>(Name, LightPosition, TargetPos, LightIntensity, Type);
		if (pNewLight)
		{
			Lights.push_back(pNewLight);
		}
	}

	void SceneManager::DeleteObject(const std::string &sprite_name) noexcept
	{
		auto it = MeshMap.find(sprite_name);
		if (it != MeshMap.end())
			MeshMap.erase(it);
	}

	std::shared_ptr<Mesh> SceneManager::GetMesh(const std::string &MeshName) const noexcept
	{
		auto it = MeshMap.find(MeshName);

		if (it != MeshMap.end())
			return it->second;
		else
			return nullptr;
	}

	const std::unordered_map<std::string, std::shared_ptr<Mesh>> &SceneManager::GetMeshMap() const noexcept
	{
		return MeshMap;
	}
}
