#ifndef CXC_ANIMATION_H
#define CXC_ANIMATION_H

#include "Core/EngineTypes.h"

namespace cxc
{
	class AnimLayer;
	enum class eAnimPlayMode : uint16_t;

	/*
		AnimStack is a collection of AnimLayer, different AnimLayer can be blended
	*/
	class CXC_ENGINECORE_API AnimStack final
	{
	public:

		AnimStack(const std::string& Name);
		~AnimStack();

	public:

		std::string GetAnimStackName() { return AnimationStackName; }

	public:

		void AddAnimationLayer(std::shared_ptr<AnimLayer> pAnimLayer);
		void RemoveAnimationLayer(size_t Index);

	public:

		void Evaluate(float CurrentTime, eAnimPlayMode PlayMode, const std::vector<glm::vec3>& SrcVertices, std::vector<glm::vec3>& DstVertices);
		void SetCurrentActiveAnimLayer(size_t LayerIndex);
		std::shared_ptr<AnimLayer> GetCurrentActiveAnimLayer() { return pCurrentActiveAnimLayer; }

	private:

		// Current active animation layer
		std::shared_ptr<AnimLayer> pCurrentActiveAnimLayer;

		// Animation layers the animation stack has
		std::vector<std::shared_ptr<AnimLayer>> AnimLayers;

		// Name of the animation stack
		std::string AnimationStackName;
	};
}

#endif // CXC_ANIMATION_H
