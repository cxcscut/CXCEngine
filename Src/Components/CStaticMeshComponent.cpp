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
		pStaticMesh = nullptr;
	}

	void CStaticMeshComponent::Tick(float DeltaSeconds)
	{
	
	}
}