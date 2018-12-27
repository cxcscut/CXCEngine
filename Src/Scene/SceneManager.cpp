#include "Scene/SceneManager.h"
#include "World/World.h"
#include <iostream>

namespace cxc {

	SceneManager::SceneManager()
		: m_ObjectMap(),
		m_Boundary()
	{
		pMaterialMgr = MaterialManager::GetInstance();
		pRendererMgr = RendererManager::GetInstance();
	}

	SceneManager::~SceneManager()
	{
		m_ObjectMap.clear();
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

	void SceneManager::UpdateBoundary(const CXCRect3 &AABB) noexcept
	{
		m_Boundary.max.x = std::fmax(m_Boundary.max.x, AABB.max.x);
		m_Boundary.max.y = std::fmax(m_Boundary.max.y, AABB.max.y);
		m_Boundary.max.z = std::fmax(m_Boundary.max.z, AABB.max.z);

		m_Boundary.min.x = std::fmin(m_Boundary.min.x, AABB.min.x);
		m_Boundary.min.y = std::fmin(m_Boundary.min.y, AABB.min.y);
		m_Boundary.min.z = std::fmin(m_Boundary.min.z, AABB.min.z);
	}

	void SceneManager::AddObject(const std::shared_ptr<CObject> &ObjectPtr) noexcept
	{
		if(ObjectPtr != nullptr)
			m_ObjectMap.insert(std::make_pair(ObjectPtr->GetName(), ObjectPtr));
	}

	void SceneManager::Tick(float DeltaSeconds) noexcept
	{
		// Draw the scene
		RenderScene();

		for (auto pObject : m_ObjectMap)
		{
			// Tick
			pObject.second->Tick(DeltaSeconds);
		}
	}

	void SceneManager::ProcessSceneNode(FbxNode* pRootNode, std::vector<std::shared_ptr<Mesh>>& OutMeshes)  noexcept
	{
		if (!pRootNode)
			return;

		if (pRootNode->GetNodeAttribute() != nullptr)
		{
			auto pPhysicalWorld = PhysicalWorld::GetInstance();
			FbxNodeAttribute::EType AttributeType;
			AttributeType = pRootNode->GetNodeAttribute()->GetAttributeType();

			switch (AttributeType)
			{
			default:
				break;

			case FbxNodeAttribute::eMesh:
			{
				std::vector<std::shared_ptr<Mesh>> LoadedMeshes;
				FbxAMatrix lGlobalPosition;
				bool bMeshLoadingRes = FBXSDKUtil::GetObjectFromNode(pRootNode, LoadedMeshes, pPhysicalWorld->GetWorldID(), pPhysicalWorld->GetTopSpaceID(), lGlobalPosition);
				if (!bMeshLoadingRes)
				{
					std::cout << "SceneManager::ProcessSceneNode, Failed to load the mesh" << std::endl;
				}
				else
				{
					for (auto pMeshes : LoadedMeshes)
					{
						OutMeshes.emplace_back(pMeshes);
					}
				}
				break;
			}

			case FbxNodeAttribute::eLight:
			{
				std::vector<std::shared_ptr<LightSource>> LoadedLights;
				FbxAMatrix lGlobalPosition;
				bool bLightLoadingRes = FBXSDKUtil::GetLightFromRootNode(pRootNode, LoadedLights, lGlobalPosition);
				if (!bLightLoadingRes)
				{
					std::cout << "SceneManager::ProcessSceneNode, Failed to load the lights" << std::endl;
				}
				else
				{
					for (auto pNewLight : LoadedLights)
					{
						AddLight(pNewLight);
					}
				}

				break;
			}

			}
		}

		// Process the child node
		int ChildNodeCount = pRootNode->GetChildCount();
		for (int i = 0; i < ChildNodeCount; ++i)
		{
			ProcessSceneNode(pRootNode->GetChild(i), OutMeshes);
		}
	}

	bool SceneManager::LoadSceneFromFBX(const std::string& filepath, std::vector<std::shared_ptr<Mesh>>& OutMeshes) noexcept
	{
		FbxManager* pSdkManager = nullptr;
		FbxScene* pScene = nullptr;
		bool bSuccessfullyLoadedScene = false;

		// Initialize the FBX SDK
		FBXSDKUtil::InitializeSDKObjects(pSdkManager, pScene);
		bSuccessfullyLoadedScene = FBXSDKUtil::LoadScene(pSdkManager, pScene, filepath.c_str());
		if (!bSuccessfullyLoadedScene)
		{
			FBXSDKUtil::DestroySDKObjects(pSdkManager, bSuccessfullyLoadedScene);
			return false;
		}
		
		// Process node from the root node of the scene
		ProcessSceneNode(pScene->GetRootNode(), OutMeshes);

		// Destroy all the objects created by the FBX SDK
		FBXSDKUtil::DestroySDKObjects(pSdkManager, bSuccessfullyLoadedScene);

		return true;
	}

	void SceneManager::RenderScene() noexcept
	{
		// Clear the renderer context for the current rendering
		pRendererMgr->ClearRendererContext();

		// Allocates buffers for rendering
		for (auto pObject : m_ObjectMap)
		{
			auto StaticMeshComponent = pObject.second->GetComponent<CStaticMeshComponent>();
			if (pObject.second->IsEnable() && StaticMeshComponent)
			{
				// Allocate the buffers
				StaticMeshComponent->AllocateBuffers();

				// Add sub-meshes to the render context
				size_t SubMeshCount = StaticMeshComponent->GetStaticMesh()->GetSubMeshCount();
				for (size_t Index = 0; Index < SubMeshCount; ++Index)
				{
					auto pSubMesh = StaticMeshComponent->GetStaticMesh()->GetSubMesh(Index);
					pRendererMgr->AddSubMeshToRendererContext(pSubMesh);
				}
			}
		}

		// Rendering scene
		for (auto pRendererIter : pRendererMgr->RenderersMap)
		{
			auto RendererContextIter = pRendererMgr->RendererContextsMap.find(pRendererIter.second->GetRendererName());
			if (RendererContextIter != pRendererMgr->RendererContextsMap.end())
			{
				pRendererMgr->UseRenderer(pRendererIter.second);
				pRendererIter.second->Render(RendererContextIter->second, Lights);
			}
		}

		// Release buffers
		for (auto pObject : m_ObjectMap)
		{
			auto StaticMeshComponent = pObject.second->GetComponent<CStaticMeshComponent>();
			if (pObject.second->IsEnable() && StaticMeshComponent)
			{
				// Allocate the buffers
				StaticMeshComponent->ReleaseBuffers();
			}
		}
	}

	void SceneManager::RemoveLight(const std::string& LightName)
	{
		for (auto iter = Lights.begin(); iter != Lights.end(); ++iter)
		{
			if ((*iter)->GetLightName() == LightName)
			{
				Lights.erase(iter);
				Lights.shrink_to_fit();
			}
		}
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

	void SceneManager::AddCamera(std::shared_ptr<Camera> pNewCamera)
	{
		pCameras.insert(std::make_pair(pNewCamera->CameraName, pNewCamera));
	}

	std::shared_ptr<Camera> SceneManager::GetCamera(const std::string& CameraName)
	{
		auto CameraIter = pCameras.find(CameraName);
		if (CameraIter != pCameras.end())
		{
			return CameraIter->second;
		}
		else
			return nullptr;
	}

	std::shared_ptr<Camera> SceneManager::GetCurrentActiveCamera()
	{
		return CurrentActiveCamera;
	}

	void SceneManager::SetCameraActive(const std::string& CameraName)
	{
		auto pCamera = GetCamera(CameraName);
		if (pCamera)
		{
			CurrentActiveCamera = pCamera;
		}
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
		auto it = m_ObjectMap.find(sprite_name);
		if (it != m_ObjectMap.end())
			m_ObjectMap.erase(it);
	}

	std::shared_ptr<CObject> SceneManager::GetObject(const std::string &ObjectName) const noexcept
	{
		auto it = m_ObjectMap.find(ObjectName);

		if (it != m_ObjectMap.end())
			return it->second;
		else
			return std::shared_ptr<CObject>(nullptr);
	}

	const std::unordered_map<std::string, std::shared_ptr<CObject>> &SceneManager::GetObjectMap() const noexcept
	{
		return m_ObjectMap;
	}
}
