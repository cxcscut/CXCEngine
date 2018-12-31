#include "Core/CObject.h"
#include "World/World.h"

namespace cxc
{
	CObject::CObject()
	{

	}

	CObject::~CObject()
	{
		
	}

	void CObject::Tick(float DeltaSeconds)
	{

	}

	std::shared_ptr<World> CObject::GetWorld()
	{
		return World::GetInstance();
	}
}