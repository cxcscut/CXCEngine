#include "Core/CObject.h"
#include "Core/GUIDGenerator.h"
#include "World/World.h"
#include "World/LogicFramework.h"

namespace cxc
{
	CObject::CObject()
	{
		OwnerLogicFramework.reset();

		OwnerLogicFramework = GetWorld()->GetLogicWorld();

		GUID = GUIDGenerator::GetInstance()->AllocateGUID();
	}

	CObject::~CObject()
	{
		OwnerLogicFramework.reset();
	}

	void CObject::Tick(float DeltaSeconds)
	{

	}

	std::shared_ptr<World> CObject::GetWorld()
	{
		return World::GetInstance();
	}

	std::shared_ptr<LogicFramework> CObject::GetLogicFramework()
	{
		if (!OwnerLogicFramework.expired())
			return OwnerLogicFramework.lock();
		else
			return nullptr;
	}
}