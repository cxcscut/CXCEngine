#include "Actor/CPawn.h"
#include "World/World.h"

namespace cxc
{
	CPawn::CPawn():
		CActor()
	{
		
	}

	CPawn::~CPawn()
	{

	}

	CPawn::CPawn(const std::string &name)
		: CActor(name)
	{

	}

	void CPawn::Tick(float DeltaSeconds)
	{
		CActor::Tick(DeltaSeconds);
	}
}