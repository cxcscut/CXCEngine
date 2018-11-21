#ifdef WIN32

#include "World\World.h"

#else

#include "World/World.h"

#endif

#ifndef CXC_ENGINE_H
#define CXC_ENGINE_H

// Using double precision for the Physics engine
#define dDOUBLE

namespace cxc
{
	using DisplayConfig = struct DisplayConfig
	{
		DisplayConfig() :
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

	using RenderConfig = struct RenderConfig
	{
		// Render name
		std::string RenderName;

		// Vertex shader path
		std::string VertexShaderPath;

		// Fragment shader path
		std::string FragmentShaderPath;
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

		static void ConfigureEngineDisplaySettings(const DisplayConfig& DisplayConf);
		static void UseRender(const RenderConfig& RenderConf);
		static void InitializeEngine();
		static void StartEngine();
	};
}

#endif // CXC_ENGINE_H