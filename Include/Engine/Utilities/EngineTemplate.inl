#include <memory>

#ifndef CXC_ENGINETEMPLATE_H
#define CXC_ENGINETEMPLATE_H

namespace cxc
{
	/* NewObject is the factory template to create the object in RAII way, 
		which should be used in the engine code to avoid using new operator
	*/
	template<class FactoryClass, typename ...Argv>
	std::shared_ptr<FactoryClass> NewObject(Argv... args)
	{
		return std::make_shared<FactoryClass>(std::forward<Argv>(args)...);
	}
}

#endif // CXC_ENGINETEMPLATE_H