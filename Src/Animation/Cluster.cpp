#include "Animation/Cluster.h"
#include "Animation/LinkBone.h"

namespace cxc
{
	CCluster::CCluster()
	{

	}

	CCluster::~CCluster()
	{

	}

	std::shared_ptr<CLinkBone> CCluster::GetLinkBone()
	{
		if (!CLinkBone.expired())
			return CLinkBone.lock();
		else
			return nullptr;
	}

	std::shared_ptr<CSkin> CCluster::GetSkin()
	{
		if (!OwnerSkin.expired())
			return OwnerSkin.lock();
		else
			return nullptr;
	}

	glm::mat4 CCluster::ComputeClusterDeformation(const glm::mat4& CurrentModelMatrix)
	{
		if (ClusterMode == eClusterMode::Additive)
		{

		}
		else
		{
			auto pLinkBone = GetLinkBone();

		}

		return glm::mat4(1.0f);
	}
}