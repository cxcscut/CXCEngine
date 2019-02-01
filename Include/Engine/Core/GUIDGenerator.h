#ifndef CXC_GUIDGENERATOR_H
#define CXC_GUIDGENERATOR_H

#include "EngineCore.h"
#include "Utilities/Singleton.inl"

namespace cxc
{
	/* GUIDGenerator is the class helps to generate the global unique ID of CObject */
	class CXC_ENGINECORE_API GUIDGenerator :public Singleton<GUIDGenerator>
	{
	public:
		friend class Singleton<GUIDGenerator>;

		GUIDGenerator();
		~GUIDGenerator();

	public:

		uint32_t AllocateGUID();

	private:

		uint32_t AllocatedGUIDNum;
	};
}

#endif // CXC_GUIDGENERATOR_H