#ifndef CXC_CSKELETON_H
#define CXC_CSKELETON_H

#include "Core/EngineCore.h"
#include "Core/EngineTypes.h"

namespace cxc
{
	class CLinkBone;

	/* The Skeleton class represents a skin-meshed skeleton */
	class CXC_ENGINECORE_API CSkeleton
	{
	public:
		CSkeleton(const std::string& Name);
		~CSkeleton();

	public:

		void SetRootBone(std::shared_ptr<CLinkBone> pRoot) { RootBone = pRoot; }
		std::string GetSkeletonName() const { return SkeletonName; }

	private:

		// Name of the skeleton
		std::string SkeletonName;

		// Root bone
		std::shared_ptr<CLinkBone> RootBone;
	};
}

#endif // CXC_CSKELETON_H