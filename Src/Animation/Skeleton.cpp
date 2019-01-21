#include "Animation/Skeleton.h"
#include "Animation/LinkBone.h"

namespace cxc
{
	CSkeleton::CSkeleton(const std::string& Name):
		SkeletonName(Name), RootBone(nullptr)
	{

	}

	CSkeleton::~CSkeleton()
	{

	}

	void CSkeleton::SetRootBone(std::shared_ptr<CLinkBone> pRoot)
	{
		RootBone = pRoot;
		pRoot->SetOwnerSkeleton(shared_from_this());
		Bones.insert(std::make_pair(pRoot->GetName(), pRoot));
	}

	bool CSkeleton::AddBone(const std::string& ParentBoneName, std::shared_ptr<CLinkBone> pNewBone)
	{
		auto ParentBone = FindBone(ParentBoneName);
		if (pNewBone && ParentBone != nullptr)
		{
			Bones.insert(std::make_pair(pNewBone->GetName(), pNewBone));
			pNewBone->SetOwnerSkeleton(shared_from_this());
			ParentBone->AddChildBone(pNewBone);

			return true;
		}

		return false;
	}

	std::shared_ptr<CLinkBone> CSkeleton::FindBone(const std::string& TargetBoneName)
	{
		auto BoneIter = Bones.find(TargetBoneName);
		if (BoneIter != Bones.end())
			return BoneIter->second;
		else
			return nullptr;
	}
}