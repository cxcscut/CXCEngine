#include "Scene/SceneContext.h"

namespace cxc
{
	SceneContext::SceneContext()
	{
	}

	SceneContext::~SceneContext()
	{
		ClearCache();
	}

	void SceneContext::ClearCache()
	{
		Lights.clear();
		Meshes.clear();
		Cameras.clear();
		Skeletons.clear();
		AnimationStacks.clear();
		Poses.clear();
	}
}