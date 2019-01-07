#ifndef CXC_CSKELETON_H
#define CXC_CSKELETON_H

#include "Core/EngineCore.h"
#include "Core/EngineTypes.h"

namespace cxc
{
	class LinkBone;

	/* The Skeleton class represents a skin-meshed skeleton */
	class CXC_ENGINECORE_API Skeleton
	{
	public:
		Skeleton();
		~Skeleton();

	public:


	private:

		// LinkeBones of the skeleton
		std::vector<LinkBone> LinkBones;

	};
}

#endif // CXC_CSKELETON_H