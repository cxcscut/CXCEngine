#include "Components/CStaticMeshComponent.h"
#include "Animation/AnimContext.h"
#include "Geometry/Mesh.h"

namespace cxc
{
	CStaticMeshComponent::CStaticMeshComponent() :
		pStaticMesh(nullptr)
	{

	}

	CStaticMeshComponent::CStaticMeshComponent(std::shared_ptr<Mesh> pMesh)
		: pStaticMesh(pMesh)
	{
	}

	CStaticMeshComponent::~CStaticMeshComponent()
	{

	}

	void CStaticMeshComponent::Tick(float DeltaSeconds)
	{
		// Animating
		auto pAnimContext = pStaticMesh->GetAnimationContext();
		if (pAnimContext)
			pAnimContext->Tick(DeltaSeconds);
	}

	void CStaticMeshComponent::AllocateBuffers()
	{
		if (pStaticMesh)
		{
			pStaticMesh->InitBuffers();
		}
	}

	void CStaticMeshComponent::ReleaseBuffers()
	{
		if (pStaticMesh)
		{
			pStaticMesh->ReleaseBuffers();
		}
	}
}