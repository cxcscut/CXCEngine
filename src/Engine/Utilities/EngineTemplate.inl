
#ifdef WIN32

#include "..\General\DefineTypes.h"

#else

#include "../General/DefineTypes.h"

#endif

#ifndef CXC_ENGINETEMPLATE_H
#define CXC_ENGINETEMPLATE_H

namespace cxc
{
	template<class FactoryClass, typename ...Argv>
	std::shared_ptr<FactoryClass> NewObject(Argv... args)
	{
		return std::make_shared<FactoryClass>(std::forward<Argv>(args)...);
	}
}

#endif // CXC_ENGINETEMPLATE_H