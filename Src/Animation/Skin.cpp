#include "Animation/Skin.h"

namespace cxc
{
	Skin::Skin()
	{

	}

	Skin::~Skin()
	{

	}

	std::shared_ptr<LinkBone> Skin::GetLinkBone()
	{
		if (!LinkBone.expired())
			return LinkBone.lock();
		else
			return nullptr;
	}

	std::shared_ptr<Cluster> Skin::GetCluster(uint32_t Index)
	{
		if (Index >= Clusters.size())
			return nullptr;
		else
			return Clusters[Index];
	}
}