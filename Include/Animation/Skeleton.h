#ifndef CXC_CSKELETON_H
#define CXC_CSKELETON_H

#include "Core/EngineCore.h"
#include "Core/EngineTypes.h"

namespace cxc
{
	class CLinkBone;

	/* The Skeleton class represents a skin-meshed skeleton */
	class CXC_ENGINECORE_API CSkeleton : public std::enable_shared_from_this<CSkeleton>
	{
	public:
		CSkeleton(const std::string& Name);
		~CSkeleton();

	public:

		size_t GetBoneCount() const { return Bones.size(); }
		std::shared_ptr<CLinkBone> GetBone(size_t Index);

		void SetRootBone(std::shared_ptr<CLinkBone> pRoot);
		std::string GetSkeletonName() const { return SkeletonName; }

		bool AddBone(const std::string& ParentBoneName, std::shared_ptr<CLinkBone> pNewBone);
		std::shared_ptr<CLinkBone> FindBone(const std::string& TargetBoneName);

	private:

		// Name of the skeleton
		std::string SkeletonName;

		// Root bone
		std::shared_ptr<CLinkBone> RootBone;

		// Bones that belongs to the skeleton
		std::vector<std::shared_ptr<CLinkBone>> Bones;
	};
}

#endif // CXC_CSKELETON_H