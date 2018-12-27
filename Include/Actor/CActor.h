#ifndef CXC_CACTOR_H
#define CXC_CACTOR_H

#include "CObject.h"
#include "General/DefineTypes.h"

namespace cxc
{
	/* CActor is the class of a plceable object in the world, which can be moved around or rotated and scaled */
	class CActor : public CObject
	{
	public:
		CActor();
		CActor(const std::string& Name);
		virtual ~CActor();

	public:

		virtual void Tick(float DeltaSeconds) override;
	};
}

#endif // CXC_CACTOR_H