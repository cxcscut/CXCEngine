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
		if (pRoot)
		{
			for (auto BoneIter = Bones.begin(); BoneIter != Bones.end(); ++BoneIter)
			{
				if (*BoneIter == pRoot)
				{
					Bones.erase(BoneIter);
					break;
				}
			}
		}

		RootBone = pRoot;
		pRoot->SetOwnerSkeleton(shared_from_this());
		Bones.push_back(pRoot);
	}

	bool CSkeleton::AddBone(const std::string& ParentBoneName, std::shared_ptr<CLinkBone> pNewBone)
	{
		auto ParentBone = FindBone(ParentBoneName);
		if (pNewBone && ParentBone != nullptr)
		{
			Bones.push_back(pNewBone);
			pNewBone->SetOwnerSkeleton(shared_from_this());
			ParentBone->AddChildBone(pNewBone);

			return true;
		}

		return false;
	}

	std::shared_ptr<CLinkBone> CSkeleton::FindBone(const std::string& TargetBoneName)
	{
		for (auto pBone : Bones)
		{
			if (pBone->GetName() == TargetBoneName)
				return pBone;
		}

		return nullptr;
	}

	std::shared_ptr<CLinkBone> CSkeleton::GetBone(size_t Index)
	{
		if (Index >= Bones.size())
			return nullptr;
		else
			return Bones[Index];
	}
}