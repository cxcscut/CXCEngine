#ifndef CXC_ANIMATIONCONTEX_H
#define CXC_ANIMATIONCONTEX_H

#include "Core/EngineTypes.h"

namespace cxc
{
	class Mesh;
	class AnimStack;
	class Skeleton;
	class CAnimComponent;

	enum class eAnimPlayMode : uint16_t
	{
		PlayOnce = 0, /* The animation will play only once and stop */
		Looping = 1 /* The animation will loop */
	};

	/* 
		AnimContext is the instance that stores the runtime information of a AnimStack, 
		such as current time of the animation, the current active AnimStack
	*/
	class CXC_ENGINECORE_API AnimContext final
	{
	public:
		AnimContext(std::shared_ptr<CAnimComponent> pAnimComponent);
		~AnimContext();

	public:

		void SetCurrentActiveAnimStack(std::shared_ptr<AnimStack> pAnimStack) { pCurrentActiveAnimStack = pAnimStack; }
		void SetPlayMode(eAnimPlayMode Mode) { PlayMode = Mode; }

		eAnimPlayMode GetPlayMode() const { return PlayMode; }
		std::shared_ptr<CAnimComponent> GetOwnerComponent();
		std::shared_ptr<AnimStack> GetCurrentActiveAnimStack() { return pCurrentActiveAnimStack; }
		std::vector<glm::vec3>&  GetDeformedVertices() { return DeformedVertices; }

		void ResetTime() { CurrentTime = 0.0f; }

	public:

		void Tick(float DeltaSeconds);

	private:

		// Play mode of the animation
		eAnimPlayMode PlayMode;

		// Current time of the animation contex
		float CurrentTime;

		// Weak pointer back to the Mesh that owns the context
		std::weak_ptr<CAnimComponent> pOwnerComponent;

		// Current active animation stack
		std::shared_ptr<AnimStack> pCurrentActiveAnimStack;

		// CSkin-meshes skeleton that the animation is currently using
		std::shared_ptr<Skeleton> pSkeleton;

		// Vertices deformed during the animation
		std::vector<glm::vec3> DeformedVertices;
	};
}

#endif // CXC_ANIMATIONCONTEX_H