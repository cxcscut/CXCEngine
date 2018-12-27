#ifndef CXC_PAWN_H
#define CXC_PAWN_H

#include "Actor/CActor.h"
#include "Physics/RigidBody3D.h"

namespace cxc
{
	/* CPawn is an CActor that can simulate physics */
	class CPawn : public CActor , public RigidBody3D
	{
	public :
		CPawn();
		CPawn(const std::string& Name);
		virtual ~CPawn();

	public:

		virtual void Tick(float DeltaSeconds) override;

	public:

		//virtual void PhysicsTick(float DeltaSeconds);

	};
}

#endif // CXC_PAWN_H