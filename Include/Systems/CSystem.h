#ifndef CXC_CSYSTEM_H
#define CXC_CSYSTEM_H

#include "General/EngineCore.h"

namespace cxc
{
	/* CSystem is the bass class for all the system in ECS architecture */
	class CXC_ENGINECORE_API CSystem
	{
	public:

		CSystem();
		virtual ~CSystem();
	};
}

#endif // CXC_CSYSTEM_H