#ifndef CXC_CSKIN_H
#define CXC_CSKIN_H

#include "Core/EngineCore.h"
#include "Core/EngineTypes.h"

namespace cxc
{
	class Cluster;
	class LinkBone;

	/* The Skin is a class that contains a collection of CClusters */
	class CXC_ENGINECORE_API Skin
	{
	public:
		Skin();
		~Skin();

	public:

		uint32_t GetClusterCount() const { return Clusters.size(); }
		std::shared_ptr<Cluster> GetCluster(uint32_t Index);
		std::shared_ptr<LinkBone> GetLinkBone();

	private:

		// Clusters
		std::vector<std::shared_ptr<Cluster>> Clusters;

		// Weak pointer back to the bone that the skin attached to
		std::weak_ptr<LinkBone> LinkBone;
	};
}


#endif // CXC_CSKIN_H