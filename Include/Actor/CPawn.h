#ifndef CXC_PAWN_H
#define CXC_PAWN_H

#include "Actor/CActor.h"

namespace cxc
{
	class CRigidBodyComponent;

	/* CPawn is an CActor that can simulate physics */
	class CXC_ENGINECORE_API CPawn : public CActor
	{
	public :
		CPawn();
		CPawn(const std::string& Name);
		virtual ~CPawn();

	public:

		virtual void Initialize() override;

	public:

		virtual void Tick(float DeltaSeconds) override;
	};
}

#endif // CXC_PAWN_H