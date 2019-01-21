#include "Animation/Skin.h"

namespace cxc
{
	CSkin::CSkin()
	{

	}

	CSkin::~CSkin()
	{

	}

	std::shared_ptr<CLinkBone> CSkin::GetLinkBone()
	{
		if (!CLinkBone.expired())
			return CLinkBone.lock();
		else
			return nullptr;
	}

	std::shared_ptr<CCluster> CSkin::GetCluster(uint32_t Index)
	{
		if (Index >= Clusters.size())
			return nullptr;
		else
			return Clusters[Index];
	}
}