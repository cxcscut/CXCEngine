#include "Animation/Cluster.h"
#include "Animation/LinkBone.h"

namespace cxc
{
	CCluster::CCluster()
	{
		pOwnerBone.reset();
	}

	CCluster::~CCluster()
	{
		pOwnerBone.reset();
	}

	std::shared_ptr<CLinkBone> CCluster::GetLinkBone()
	{
		if (!pOwnerBone.expired())
			return pOwnerBone.lock();
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