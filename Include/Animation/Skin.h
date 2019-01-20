#ifndef CXC_CSKIN_H
#define CXC_CSKIN_H

#include "Core/EngineCore.h"
#include "Core/EngineTypes.h"

namespace cxc
{
	class CCluster;
	class CLinkBone;

	/* The CSkin is a class that contains a collection of CClusters */
	class CXC_ENGINECORE_API CSkin
	{
	public:
		CSkin();
		~CSkin();

	public:

		uint32_t GetClusterCount() const { return Clusters.size(); }
		std::shared_ptr<CCluster> GetCluster(uint32_t Index);
		std::shared_ptr<CLinkBone> GetLinkBone();

	private:

		// Clusters
		std::vector<std::shared_ptr<CCluster>> Clusters;

		// Weak pointer back to the bone that the skin attached to
		std::weak_ptr<CLinkBone> CLinkBone;
	};
}


#endif // CXC_CSKIN_H