#ifndef CXC_SCENECONTEXT_H
#define CXC_SCENECONTEXT_H

#include "Core/EngineCore.h"
#include <memory>
#include <vector>

namespace cxc
{
	class Mesh;
	class LightSource;
	class Camera;
	class AnimStack;
	class CSkeleton;
	class CPose;

	/* SceneContext is the class that stores the information of loaded scene */
	class CXC_ENGINECORE_API SceneContext final
	{
	public:
		SceneContext();
		~SceneContext();

	public:

		void ClearCache();

	public:

		// Meshes
		std::vector<std::shared_ptr<Mesh>> Meshes;

		// Lights
		std::vector<std::shared_ptr<LightSource>> Lights;

		// Cameras
		std::vector<std::shared_ptr<Camera>> Cameras;

		// Poses
		std::vector<std::shared_ptr<CPose>> Poses;

		// Animation stacks
		std::vector<std::shared_ptr<AnimStack>> AnimationStacks;

		// Skeletons
		std::vector<std::shared_ptr<CSkeleton>> Skeletons;
	};
}

#endif // CXC_SCENECONTEXT_H