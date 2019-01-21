#include "Animation/LinkBone.h"

namespace cxc
{
	CLinkBone::CLinkBone(const std::string& BoneName, const glm::vec3& StartPos, const glm::vec3& EndPos):
		StartPosition(StartPos), EndPosition(EndPos), Name(BoneName)
	{
		ParentBone.reset();
	}

	CLinkBone::~CLinkBone()
	{
		ParentBone.reset();
	}

	std::shared_ptr<CSkeleton> CLinkBone::GetOwnerSkeleton()
	{
		if (!pOwnerSkeleton.expired())
			return pOwnerSkeleton.lock();
		else
			return nullptr;
	}

	void CLinkBone::SetParentBone(std::shared_ptr<CLinkBone> pParentBone)
	{
		ParentBone = pParentBone;
	}

	std::shared_ptr<CLinkBone> CLinkBone::GetParentBone()
	{
		if (!ParentBone.expired())
			return ParentBone.lock();
		else
			return nullptr;
	}

	void CLinkBone::AddChildBone(std::shared_ptr<CLinkBone> pChildBone)
	{
		if (pChildBone)
		{
			ChildBones.push_back(pChildBone);
			pChildBone->SetParentBone(shared_from_this());
		}
	}
}