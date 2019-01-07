#ifndef CXC_COBJECT_H
#define CXC_COBJECT_H

#include "Core/EngineCore.h"
#include <memory>

namespace cxc
{
	class World;
	class LogicFramework;

	/* CObject is the base class for all the engine core class */
	class CXC_ENGINECORE_API CObject
	{
	public:
		friend class LogicFramework;

		CObject();
		virtual ~CObject();

	public:

		virtual void Tick(float DeltaSeconds);

	public:

		std::shared_ptr<LogicFramework> GetLogicFramework();
		std::shared_ptr<World> GetWorld();
		uint32_t GetObjectGUID() const { return GUID; }

	private:

		// Global unique ID of the CObject, 0 represents the invalid ID
		uint32_t GUID;

		// Weak pointer back to the logic framework
		std::weak_ptr<LogicFramework> OwnerLogicFramework;
	};
}

#endif // CXC_COBJECT_H