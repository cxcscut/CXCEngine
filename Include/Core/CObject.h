#ifndef CXC_COBJECT_H
#define CXC_COBJECT_H

#include "Core/EngineCore.h"

namespace cxc
{
	/* CObject is the base class for all the engine core class */
	class CXC_ENGINECORE_API CObject
	{
	public:
		CObject();
		virtual ~CObject();

	public:

		virtual void Tick(float DeltaSeconds);
	};
}

#endif // CXC_COBJECT_H