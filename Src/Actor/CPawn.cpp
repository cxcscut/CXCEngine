#include "Actor/CPawn.h"
#include "World/World.h"
#include "Components/CRigidBodyComponent.h"

namespace cxc
{
	CPawn::CPawn():
		CActor()
	{
		// Create RigidBodyComponent and attach it to the pawn
		auto RigidBodyComponent = NewObject<CRigidBodyComponent>();
		SetRootComponent<CRigidBodyComponent>(RigidBodyComponent);
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