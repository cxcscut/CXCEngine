#include "Animation/AnimContext.h"
#include "Animation/AnimStack.h"
#include "Animation/AnimLayer.h"
#include "Geometry/Mesh.h"

namespace cxc
{
	AnimContext::AnimContext(std::shared_ptr<Mesh> pObject):
		pCurrentActiveAnimStack(nullptr), CurrentTime(0.0f)
	{
		pOwnerMesh.reset();
		pOwnerMesh = pObject;
	}

	AnimContext::~AnimContext()
	{

	}

	void AnimContext::Tick(float DeltaSeconds)
	{
		if (pOwnerMesh.expired())
			return;

		if (pCurrentActiveAnimStack && pOwnerMesh.lock())
		{
			CurrentTime += DeltaSeconds;

			auto SrcVertices = pOwnerMesh.lock()->m_VertexCoords;
			pCurrentActiveAnimStack->Evaluate(CurrentTime, PlayMode, SrcVertices, DeformedVertices);
		}
	}

	void AnimContext::SetOwnerObject(std::shared_ptr<Mesh> pObject)
	{
		pOwnerMesh = pObject;
	}

	std::shared_ptr<Mesh> AnimContext::GetOwnerCActor()
	{
		return pOwnerMesh.lock();
	}
}