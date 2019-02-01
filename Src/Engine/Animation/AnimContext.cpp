#include "Animation/AnimContext.h"
#include "Animation/AnimStack.h"
#include "Animation/AnimLayer.h"
#include "Geometry/Mesh.h"
#include "Components/CAnimComponent.h"
#include "Components/CStaticMeshComponent.h"
#include "Actor/CActor.h"

namespace cxc
{
	AnimContext::AnimContext(std::shared_ptr<CAnimComponent> pAnimComponent):
		pCurrentActiveAnimStack(nullptr), CurrentTime(0.0f)
	{
		pOwnerComponent.reset();
		pOwnerComponent = pAnimComponent;
	}

	AnimContext::~AnimContext()
	{

	}

	void AnimContext::Tick(float DeltaSeconds)
	{
		CurrentTime += DeltaSeconds;

		if (pOwnerComponent.expired())
			return;

		if (pCurrentActiveAnimStack && pOwnerComponent.lock())
		{
			auto OwnerActor = pOwnerComponent.lock()->GetOwnerObject();
			assert(OwnerActor != nullptr);

			// Animating static mesh
			size_t ComponentCount = OwnerActor->GetComponentCount();
			for (size_t Index = 0; Index < ComponentCount; ++Index)
			{
				auto StaticMeshComponent = std::dynamic_pointer_cast<CStaticMeshComponent>(OwnerActor->GetComponent(Index));
				if (StaticMeshComponent)
				{
					auto pStaticMesh = StaticMeshComponent->GetStaticMesh();
					auto SrcVertices = pStaticMesh->m_VertexCoords;
					pCurrentActiveAnimStack->Evaluate(CurrentTime, PlayMode, SrcVertices, DeformedVertices);
				}
			}
		}
	}

	std::shared_ptr<CAnimComponent> AnimContext::GetOwnerComponent()
	{
		return pOwnerComponent.lock();
	}
}