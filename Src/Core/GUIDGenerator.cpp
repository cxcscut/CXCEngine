#include "Core/GUIDGenerator.h"

namespace cxc
{
	GUIDGenerator::GUIDGenerator():
		AllocatedGUIDNum(0)
	{

	}

	GUIDGenerator::~GUIDGenerator()
	{

	}

	uint32_t GUIDGenerator::AllocateGUID()
	{
		return ++AllocatedGUIDNum;
	}
}