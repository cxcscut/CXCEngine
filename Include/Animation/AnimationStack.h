#include "General/DefineTypes.h"

#ifndef CXC_ANIMATION_H
#define CXC_ANIMATION_H

namespace cxc
{
	class AnimLayer;

	class AnimStack final
	{
	public:
		AnimStack();
		~AnimStack();

	public:



	private:

		// Animation layers the animation stack has
		std::unordered_map<std::string, std::shared_ptr<AnimLayer>> AnimLayers;

		// Name of the animation stack
		std::string AnimationStackName;
	};
}

#endif // CXC_ANIMATION_H
