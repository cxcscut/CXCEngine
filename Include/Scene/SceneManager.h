#include "General/DefineTypes.h"
#include "Utilities/Singleton.inl"
#include "Geometry/Mesh.h"
#include "Geometry/SubMesh.h"
#include "Material/MaterialManager.h"
#include "Scene/Camera.h"
#include "Utilities/FBXSDKUtility.h"
#include "Rendering/RendererManager.h"
#include "Actor/CObject.h"
#include "Components/CStaticMeshComponent.h"

#ifndef CXC_SCENEMANAGER_H
#define CXC_SCENEMANAGER_H

#define PARTITION_MIN_NODENUM 8
#define MAX_PARTITION_DEPTH 8
#define MOVE_NORTH 1
#define MOVE_SOUTH 2
#define MOVE_EAST 3
#define MOVE_WEST 4
#define MOVE_UP 5
#define MOVE_DOWN 6

#define WOLRD_QUICK_STEPSIZE 0.02f

namespace cxc {

	class SceneManager final : public Singleton<SceneManager>
	{

	public:

		friend Singleton<SceneManager>;
		friend class World;

		explicit SceneManager();
		~SceneManager();

		SceneManager(const SceneManager&) = delete;
		SceneManager(const SceneManager&&) = delete;
		SceneManager& operator=(const SceneManager&) = delete;
		SceneManager& operator=(const SceneManager&&) = delete;

	// Scene loading from fbx file via FBX SDKs
	public :

		bool LoadSceneFromFBX(const std::string& filepath, std::vector<std::shared_ptr<Mesh>>& OutMeshes) noexcept;
		void ProcessSceneNode(FbxNode* pNode, std::vector<std::shared_ptr<Mesh>>& OutMeshes) noexcept;

	public:

		// Delete object
		void DeleteObject(const std::string &sprite_name) noexcept;

	public:

		std::shared_ptr<MaterialManager> pMaterialMgr;
		std::unordered_map<std::string, std::shared_ptr<Camera>> pCameras;
		std::shared_ptr<RendererManager> pRendererMgr;

		std::shared_ptr<CObject> GetObject (const std::string &ObjectName) const noexcept;
		const std::unordered_map<std::string, std::shared_ptr<CObject>> &GetObjectMap() const noexcept;

		// Lights
	public:

		void AddLight(const std::string& Name, const glm::vec3& LightPosition, const glm::vec3& TargetPos, float LightIntensity, eLightType Type);
		void AddLight(std::shared_ptr<LightSource> pNewLight);
		void RemoveLight(const std::string& LightName);
		std::shared_ptr<LightSource> GetLight(uint32_t LightIndex);
		std::shared_ptr<LightSource> GetLight(const std::string& LightName);
		uint32_t GetLightCount() const { return Lights.size(); }

	public:

		void AddCamera(const std::string& CameraName,
			const glm::vec3 &eye, const glm::vec3 &origin, const glm::vec3 &up,
			const glm::mat4 &ProjectionMatrix) noexcept;

		void AddCamera(std::shared_ptr<Camera> pNewCamera);
		std::shared_ptr<Camera> GetCamera(const std::string& CameraName);
		std::shared_ptr<Camera> GetCurrentActiveCamera();
		void SetCameraActive(std::shared_ptr<Camera> pCamera);
		void SetCameraActive(const std::string& CameraName);

		// Add object to object map
		void AddObject(const std::shared_ptr<CObject> &ObjectPtr) noexcept;

	public:

		void Tick(float DeltaSeconds) noexcept;

		void RenderScene() noexcept;

	private:

		void UpdateBoundary(const CXCRect3 &AABB) noexcept;

	private:

		std::shared_ptr<Camera> CurrentActiveCamera;

		// Lights
		std::vector<std::shared_ptr<LightSource>> Lights;

		// <Object Name , Pointer to object>
		std::unordered_map<std::string, std::shared_ptr<CObject>> m_ObjectMap;

		CXCRect3 m_Boundary;
	};

}
#endif // CXC_SCENEMANAGER_H
