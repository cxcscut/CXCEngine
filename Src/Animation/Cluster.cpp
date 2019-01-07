#include "Animation/Cluster.h"
#include "Animation/LinkBone.h"

namespace cxc
{
	Cluster::Cluster()
	{

	}

	Cluster::~Cluster()
	{

	}

	std::shared_ptr<LinkBone> Cluster::GetLinkBone()
	{
		if (!LinkBone.expired())
			return LinkBone.lock();
		else
			return nullptr;
	}

	std::shared_ptr<Skin> Cluster::GetSkin()
	{
		if (!OwnerSkin.expired())
			return OwnerSkin.lock();
		else
			return nullptr;
	}

	glm::mat4 Cluster::ComputeClusterDeformationMatrix()
	{
		if (ClusterMode == eClusterMode::Additive)
		{

		}
		else
		{

		}
	}
}