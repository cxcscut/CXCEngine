#ifndef CXC_COBJECT_H
#define CXC_COBJECT_H

#include "Core/EngineCore.h"
#include <memory>

namespace cxc
{
	class World;

	/* CObject is the base class for all the engine core class */
	class CXC_ENGINECORE_API CObject
	{
	public:
		CObject();
		virtual ~CObject();

	public:

		virtual void Tick(float DeltaSeconds);

	public:

		std::shared_ptr<World> GetWorld();
	};
}

#endif // CXC_COBJECT_H