#include "Actor/CActor.h"

namespace cxc
{
	CActor::CActor() :
		CObject()
	{

	}

	CActor::CActor(const std::string& Name):
		CObject(Name)
	{

	}

	CActor::~CActor()
	{

	}

	void CActor::Tick(float DeltaSeconds)
	{
		CObject::Tick(DeltaSeconds);
	}
}