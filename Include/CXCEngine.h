#include "World/World.h"

#ifndef CXC_ENGINE_H
#define CXC_ENGINE_H

// Using double precision for the Physics engine
#define dDOUBLE

namespace cxc
{
	using DisplayParameters = struct DisplayParameters
	{
		DisplayParameters() :
			WindowHeight(600), WindowWidth(800),
			ApplicationTitle("Powered by CXCEngine"),
			bIsDecorated(false), 
			WindowPosX(200), WindowPosY(200)
		{

		}

		// Position of the display window
		uint32_t WindowPosX, WindowPosY;

		// Height of the display window
		uint32_t WindowHeight;

		// Width of the display window
		uint32_t WindowWidth;

		// Title of the application
		std::string ApplicationTitle;

		// Whether the window is decorated
		bool bIsDecorated;
	};

	class GEngine final
	{

	public:

		GEngine();
		~GEngine();

	public:

		static std::shared_ptr<World> GetWorldInstance() { return World::GetInstance(); }
		static std::shared_ptr<SceneManager> GetSceneManagerInstance() { return SceneManager::GetInstance(); }

	public:

		static void ConfigureEngineDisplaySettings(const DisplayParameters& DisplayConf);
		static void BindMeshRender(std::shared_ptr<MeshRender> pRender, std::shared_ptr<Object3D> pObject, uint32_t MeshIndex);
		static void SetCamera(const glm::vec3& CameraPosition, const glm::vec3& CameraOrigin, const glm::vec3& UpVector, const glm::mat4 ProjectionMatrix);

	public:

		static void InitializeEngine();

		static void StartEngine();
	};
}

#endif // CXC_ENGINE_H