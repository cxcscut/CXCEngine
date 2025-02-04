#include "Core/EngineTypes.h"
#include "Utilities/Singleton.inl"
#include "Geometry/Mesh.h"
#include "Geometry/SubMesh.h"
#include "Material/MaterialManager.h"
#include "Scene/Camera.h"
#include "Utilities/FBXSDKUtility.h"
#include "Rendering/RendererManager.h"
#include "Actor/CActor.h"
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

	class DebugMesh;
	class AnimStack;
	class CSkeleton;
	class CPose;

	class CXC_ENGINECORE_API SceneManager final : public Singleton<SceneManager>
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

	public:

		void AllocateMeshBuffers();
		void ReleaseMeshBuffers();
		
		void CleanPendingKillMeshes(float DeltaSeconds);

	public:

		std::shared_ptr<MaterialManager> pMaterialMgr;
		std::shared_ptr<RendererManager> pRendererMgr;

		std::shared_ptr<Mesh> GetMesh (const std::string &MeshName) const noexcept;
		const std::unordered_map<std::string, std::shared_ptr<Mesh>> &GetMeshMap() const noexcept;

	public:

		void AddLight(const std::string& Name, const glm::vec3& LightPosition, const glm::vec3& TargetPos, float LightIntensity, eLightType Type);
		void AddLight(std::shared_ptr<LightSource> pNewLight);
		void RemoveLight(std::shared_ptr<LightSource> pLight);
		void RemoveCamera(std::shared_ptr<Camera> pCamera);
		const std::vector<std::shared_ptr<LightSource>>& GetLightsArray() const { return Lights; }
		std::shared_ptr<LightSource> GetLight(uint32_t LightIndex);
		std::shared_ptr<LightSource> GetLight(const std::string& LightName);
		uint32_t GetLightCount() const { return Lights.size(); }

	public:

		void AddCamera(const std::string& CameraName,
			const glm::vec3 &eye, const glm::vec3 &origin, const glm::vec3 &up,
			const glm::mat4 &ProjectionMatrix) noexcept;

		std::shared_ptr<Camera> GetCurrentActiveCamera();
		void SetCameraActive(std::shared_ptr<Camera> pCamera);

		void RemoveMesh(const std::shared_ptr<Mesh> pMesh);
		void AddMesh(const std::shared_ptr<Mesh> pMesh) noexcept;
		void AddMesh(const std::vector<std::shared_ptr<Mesh>> Meshes);
		void AddDebugMesh(std::shared_ptr<DebugMesh> pDebugMesh);

		size_t GetCameraCount() const { return Cameras.size(); }
		std::shared_ptr<Camera> GetCamera(size_t Index);
		void AddCamera(std::shared_ptr<Camera> Camera);

	public:

		void AddAnimationStack(std::shared_ptr<AnimStack> pAnimStack);
		void RemoveAnimationStack(std::shared_ptr<AnimStack> pAnimStack);

		std::shared_ptr<AnimStack> GetAnimationStack(const std::string& AnimStackName);

	public:

		void RenderingTick(float DeltaSeconds) noexcept;

		void FlushDebugMeshes();

	private:

		void UpdateBoundary(const CXCRect3D &AABB) noexcept;

	private:

		// Current active camera
		std::shared_ptr<Camera> CurrentActiveCamera;

		// Cameras in the scene
		std::vector<std::shared_ptr<Camera>> Cameras;

		// Lights
		std::vector<std::shared_ptr<LightSource>> Lights;

		// Animation stacks
		std::unordered_map<std::string, std::shared_ptr<AnimStack>> AnimationStacks;

		// Poses
		std::vector<std::shared_ptr<CPose>> Poses;

		// Skeletons
		std::unordered_map<std::string, std::shared_ptr<CSkeleton>> Skeletons;

		// <Object Name , Pointer to object>
		std::unordered_map<std::string, std::shared_ptr<Mesh>> MeshMap;

		// DebugMeshes
		std::vector<std::shared_ptr<DebugMesh>> DebugMeshes;

		// Boundary of the scene
		CXCRect3D m_Boundary;
	};

}
#endif // CXC_SCENEMANAGER_H
